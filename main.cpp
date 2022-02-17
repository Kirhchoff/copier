#include <iostream>
#include <array>
#include <stdio.h>
#include <chrono>

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

struct Arguments {
  std::string input_path {"random_file"};
  std::string output_path {"output_file"};
};

Arguments parse_args(int argc, char* argv[]) {
  Arguments retval{};
  if(argc >= 2) retval.input_path = argv[1];
  if(argc >= 3) retval.output_path = argv[2];
  return retval;
}

class Metrics {
  public:
    uint tot_size {0};
    double tot_ms {0};
    void start() {
      start_ts = std::chrono::high_resolution_clock::now();
    }
    void stop() {
      auto stop_ts = std::chrono::high_resolution_clock::now();
      tot_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(stop_ts - start_ts).count();
    }
    void processed(uint bytes) {
      tot_size += bytes;
    }

  private:
    std::chrono::high_resolution_clock::time_point start_ts;
};

int main(int argc, char* argv[]) {
  auto args = parse_args(argc, argv);
  std::cout << "Copying from " << args.input_path << " to " << args.output_path << '\n';
  Metrics metrics{};
  metrics.start();
  auto f = fopen(args.input_path.c_str(), "r");
  auto f_out = fopen(args.output_path.c_str(), "w");
  //TODO check if file empty
  for(auto location = 0u;; location++) {
    auto c = read_chunk(f, location);
    write_chunk(f_out, c);
    metrics.processed(c.size);
    if(c.last) break;
  }
  fclose(f);
  fclose(f_out);
  metrics.stop();
  std::cout << metrics.tot_size << " " << metrics.tot_ms << '\n';
  return 0;
}

