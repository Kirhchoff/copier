#include <iostream>
#include <array>
#include <stdio.h>

const uint CHUNK_SIZE = 4*1024;

struct Chunk {
  // TODO: this could just be a handle to a location in shared buffer
  using Unit_t = uint8_t;
  constexpr static size_t UNIT_SIZE = sizeof(Unit_t);
  std::array<Unit_t, CHUNK_SIZE> buffer;
  size_t size {0};
  bool last {false};
};

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


int main() {
  std::cout << "Hello\n";
  auto f = fopen("random_file", "r");
  auto f_out = fopen("output_file", "w");
  //TODO check if file empty
  for(auto location = 0u;; location++) {
    std::cout << "Reading " << location << "\n";
    auto c=read_chunk(f, location);
    write_chunk(f_out, c);
    if(c.last) break;
  }
  fclose(f);
  fclose(f_out);
  return 0;
}

