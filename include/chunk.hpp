#pragma once

#include <array>
#include <cstdint>
#include <stdio.h>

namespace copier {
const inline uint CHUNK_SIZE = 4 * 1024u;

// Object representing one chunk of transfered data
struct Chunk {
    using Unit_t = uint8_t;
    constexpr static size_t UNIT_SIZE = sizeof(Unit_t);

    // TODO: this could just be a handle to a location in a shared buffer
    std::array<Unit_t, CHUNK_SIZE> buffer{};
    size_t size{0};
    bool last{false};

    // auto operator<=>(const Chunk&) const = default; // My compiler it too old :(
    bool operator==(const Chunk &other) const;
};

// Read one chunk from file
// param file - file pointer
// param location - location in a file represented as a multiple of chunks
Chunk read_chunk(FILE *file, uint location);

// Write one chunk to file.
void write_chunk(FILE *file, const Chunk &data);
} // namespace copier
