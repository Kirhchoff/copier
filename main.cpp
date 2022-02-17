#include <iostream>
#include <stdio.h>
#include <chunk.hpp>
#include <metrics.hpp>

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

int main(int argc, char* argv[]) {
  auto args = parse_args(argc, argv);
  std::cout << "Copying from " << args.input_path << " to " << args.output_path << '\n';
  copier::Metrics metrics{};
  auto f = fopen(args.input_path.c_str(), "rb");
  auto f_out = fopen(args.output_path.c_str(), "wb");
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
  std::cout << "Copied " << metrics.total_size << " bytes in " << metrics.total_ms << " ms with average bandwidth " << metrics.bandwidth_MB() << " MB/s \n";
  return 0;
}

