#include <chunk.hpp>
#include <iostream>

namespace copier {
    Chunk read_chunk(FILE* file, uint location) {
        Chunk data;
        //TODO: handle seek errors
        fseek(file, location*CHUNK_SIZE, 0);
        size_t result = fread(data.buffer.data(), Chunk::UNIT_SIZE, CHUNK_SIZE, file);
        if(ferror(file)) {
            std::cout << "Error " << ferror(file);
            //TODO: I don't like closing a file here. Neither do i like throwing an exception. Consider different error handling.
            fclose(file);
            throw std::runtime_error("Error reading file");
        }
        data.size = result;
        data.last = result != CHUNK_SIZE;
        return data;
    }

    void write_chunk(FILE* file, const Chunk& data) {
        //TODO: handle write errors
        fwrite(data.buffer.data(), Chunk::UNIT_SIZE, data.size, file);
    }
}
