#include <iostream>
#include <stdio.h>
#include <chrono>
#include <chunk.hpp>

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
    auto c = copier::read_chunk(f, location);
    copier::write_chunk(f_out, c);
    metrics.processed(c.size);
    if(c.last) break;
  }
  fclose(f);
  fclose(f_out);
  metrics.stop();
  std::cout << "Copied " << metrics.tot_size << " bytes in " << metrics.tot_ms << " ms\n";
  return 0;
}

