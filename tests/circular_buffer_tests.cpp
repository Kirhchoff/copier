
#include <catch2/catch_test_macros.hpp>
#include <circular_buffer.hpp>
#include <chunk.hpp>

template<class Sut>
class TestBuffer : public Sut {
public:
    using Sut::read_pos;
    using Sut::write_pos;
    using Sut::free_space;
    using Sut::locked_pos;
};

TEST_CASE("Synch operations") {
    TestBuffer<copier::CircularBuffer<copier::Chunk, 3>> buf;

    SECTION("Writing one chunk") {
        copier::Chunk chunk{};
        chunk.size = 321; // dummy data
        buf.write(chunk);
        REQUIRE(buf.write_pos == 1);
        REQUIRE(buf.free_space == 2);
        SECTION("Write full buffer") {
            copier::Chunk chunk2{}, chunk3{};
            buf.write(chunk2);
            buf.write(chunk3);
            REQUIRE(buf.write_pos == 0);
            REQUIRE(buf.read_pos == 0);
            REQUIRE(buf.locked_pos == 0);
            REQUIRE(buf.free_space == 0);

            SECTION("Read chunk") {
                auto c = buf.read();
                REQUIRE(buf.write_pos == 0);
                REQUIRE(buf.read_pos == 1);
                REQUIRE(buf.locked_pos == 0);
                REQUIRE(buf.free_space == 0);
                REQUIRE(c.size == chunk.size);

                SECTION("Release chunk") {
                    buf.release();
                    REQUIRE(buf.locked_pos == 1);
                    REQUIRE(buf.free_space == 1);

                    SECTION("Write another chunk") {
                        copier::Chunk chunk4{};
                        buf.write(chunk4);
                        REQUIRE(buf.write_pos == 1);
                        REQUIRE(buf.read_pos == 1);
                        REQUIRE(buf.free_space == 0);
                    }
                }
            }
            SECTION("Read multiple before release") {
                auto c1 = buf.read();
                auto c2 = buf.read();
                REQUIRE(buf.read_pos == 2);
                REQUIRE(buf.locked_pos == 0);
                REQUIRE(buf.free_space == 0);
                buf.release();
                REQUIRE(buf.locked_pos == 2);
                REQUIRE(buf.free_space == 2);
                SECTION("Read last piece") {
                    buf.read();
                    buf.release();
                    REQUIRE(buf.read_pos == buf.write_pos);
                    REQUIRE(buf.free_space == 3);
                }
            }
        }
    }

}