#include <chunk.hpp>
#include <iostream>

namespace copier {
Chunk read_chunk(FILE *file, uint location) {
    Chunk data;
    if (fseek(file, location * CHUNK_SIZE, 0) != 0) {
        throw std::runtime_error(std::string("Error trying to read a chunk: ") + std::to_string(ferror(file)));
    }
    size_t result = fread(data.buffer.data(), Chunk::UNIT_SIZE, CHUNK_SIZE, file);
    if (ferror(file)) {
        // TODO: we could try to recover by rewinding file pointer to previous location and retrying maybe?
        throw std::runtime_error(std::string("Error reading a chunk: ") + std::to_string(ferror(file)));
    }
    data.size = result;
    data.last = result != CHUNK_SIZE;
    return data;
}

void write_chunk(FILE *file, const Chunk &data) {
    auto written = fwrite(data.buffer.data(), Chunk::UNIT_SIZE, data.size, file);
    if (written != data.size) {
        // TODO: we could try to recover by rewinding file pointer to previous location and retrying maybe?
        throw std::runtime_error(std::string("Error writing a chunk: ") + std::to_string(ferror(file)));
    }
}

bool Chunk::operator==(const Chunk &other) const {
    return buffer == other.buffer && size == other.size && last == other.last;
}
} // namespace copier
