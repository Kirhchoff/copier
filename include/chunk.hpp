#pragma once

#include <cstdint>
#include <stdio.h>
#include <array>

namespace copier {
    const inline uint CHUNK_SIZE = 4*1024u;

    struct Chunk {
        using Unit_t = uint8_t;
        constexpr static size_t UNIT_SIZE = sizeof(Unit_t);

        // TODO: this could just be a handle to a location in shared buffer
        std::array<Unit_t, CHUNK_SIZE> buffer {};
        size_t size {0};
        bool last {false};

        //auto operator<=>(const Chunk&) const = default; // My compiler it too old :(
        bool operator==(const Chunk& other) const;
    };

    Chunk read_chunk(FILE* file, uint location);

    void write_chunk(FILE* file, const Chunk& data);
}
