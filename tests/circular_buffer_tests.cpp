
#include <catch2/catch_test_macros.hpp>
#include <chunk.hpp>
#include <circular_buffer.hpp>

// Test double to expose internal data of tested class. (It is difficult to test it as black-box.)
template <class Sut> class TestBuffer : public Sut {
  public:
    using Sut::free_space;
    using Sut::read_pos;
    using Sut::write_pos;
};

constexpr int BUF_SIZE = 3;

TEST_CASE("Synch operations") {
    TestBuffer<copier::CircularBuffer<copier::Chunk, BUF_SIZE>> buf;

    REQUIRE(buf.is_empty());

    SECTION("Pop from empty buffer") {
        buf.pop();
        REQUIRE(buf.write_pos == 0);
        REQUIRE(buf.read_pos == 0);
        REQUIRE(buf.free_space == BUF_SIZE);
        REQUIRE(buf.is_empty());
    }

    SECTION("Writing one chunk") {
        copier::Chunk chunk{};
        chunk.size = 321; // dummy data
        buf.write(chunk);
        REQUIRE(buf.write_pos == 1);
        REQUIRE(buf.free_space == BUF_SIZE - 1);
        REQUIRE_FALSE(buf.is_empty());

        SECTION("Write full buffer") {
            copier::Chunk chunk2{}, chunk3{};
            buf.write(chunk2);
            buf.write(chunk3);
            REQUIRE(buf.write_pos == 0);
            REQUIRE(buf.read_pos == 0);
            REQUIRE(buf.free_space == 0);

            SECTION("Read chunk") {
                auto c = buf.peek();
                REQUIRE(buf.write_pos == 0);
                REQUIRE(buf.read_pos == 0);
                REQUIRE(buf.free_space == 0);
                REQUIRE(c == chunk);

                SECTION("Release chunk") {
                    buf.pop();
                    REQUIRE(buf.read_pos == 1);
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
                auto c1 = buf.peek();
                auto c2 = buf.peek();
                REQUIRE(buf.read_pos == 0);
                REQUIRE(c1 == c2);
                REQUIRE(buf.free_space == 0);
                buf.pop();
                REQUIRE(buf.read_pos == 1);
                REQUIRE(buf.free_space == 1);

                SECTION("Pop all pieces") {
                    buf.pop();
                    buf.pop();
                    REQUIRE(buf.read_pos == buf.write_pos);
                    REQUIRE(buf.free_space == BUF_SIZE);
                    REQUIRE(buf.is_empty());
                }
            }
        }
    }
}
