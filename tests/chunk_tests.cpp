#include <catch2/catch_test_macros.hpp>
#include <chunk.hpp>
#include <stdio.h>
#include <numeric>

TEST_CASE("Chunk operations") {
    std::FILE* tmp_file = std::tmpfile();

    SECTION("Writing full chunk") {
        copier::Chunk full_chunk;
        full_chunk.size = copier::CHUNK_SIZE;
        std::iota(full_chunk.buffer.begin(), full_chunk.buffer.end(), 1);
        copier::write_chunk(tmp_file, full_chunk);
        REQUIRE(ftell(tmp_file) == copier::CHUNK_SIZE);
        std::array<uint8_t, copier::CHUNK_SIZE> buffer;
        std::rewind(tmp_file);
        fread(buffer.data(), copier::Chunk::UNIT_SIZE, copier::CHUNK_SIZE, tmp_file);
        REQUIRE(buffer == full_chunk.buffer);
        
        SECTION("Reading full chunk") {
            std::rewind(tmp_file);
            auto out_chunk = copier::read_chunk(tmp_file, 0);
            REQUIRE(out_chunk.size == copier::CHUNK_SIZE);
            REQUIRE_FALSE(out_chunk.last);
            REQUIRE(out_chunk.buffer == full_chunk.buffer);
        }
    }

    SECTION("Writing parital chunk") {
        copier::Chunk partial_chunk;
        const auto TEST_CHUNK = 42u;
        partial_chunk.size = TEST_CHUNK;
        partial_chunk.last = true;
        std::iota(partial_chunk.buffer.begin(), partial_chunk.buffer.begin() + TEST_CHUNK, 1);
        copier::write_chunk(tmp_file, partial_chunk);
        REQUIRE(ftell(tmp_file) == TEST_CHUNK);
        std::array<uint8_t, TEST_CHUNK> buffer;
        std::rewind(tmp_file);
        fread(buffer.data(), copier::Chunk::UNIT_SIZE, TEST_CHUNK, tmp_file);
        for(auto idx=0; idx<TEST_CHUNK; idx++){
            REQUIRE(buffer[idx] == partial_chunk.buffer[idx]);
        }
        SECTION("Reading partial chunk") {
            std::rewind(tmp_file);
            auto out_chunk = copier::read_chunk(tmp_file, 0);
            REQUIRE(out_chunk.size == TEST_CHUNK);
            REQUIRE(out_chunk.last);
            REQUIRE(out_chunk.buffer == partial_chunk.buffer);
        }
    }

    fclose(tmp_file);    
}