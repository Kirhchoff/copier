#include <chunk.hpp>
#include <iostream>

namespace copier {
    Chunk read_chunk(FILE* file, uint location) {
        Chunk data;
        //TODO: handle seek errors
        fseek(file, location*CHUNK_SIZE, 0);
        size_t result = fread(data.buffer.data(), Chunk::UNIT_SIZE, CHUNK_SIZE, file);
        if(result == CHUNK_SIZE) {
            std::cout << "Read whole buffer\n";
        } else if(feof(file)) {
            std::cout << "Read " << result << " util eof\n";
        } else {
            std::cout << "Error " << ferror(file);
            //TODO: this terminates without closing files!
            throw std::runtime_error("Error reading file");
        }
        data.size = result;
        data.last = result != CHUNK_SIZE;
        return data;
    }

    void write_chunk(FILE* file, Chunk& data) {
        //TODO: handle write errors
        fwrite(data.buffer.data(), Chunk::UNIT_SIZE, data.size, file);
    }
}
