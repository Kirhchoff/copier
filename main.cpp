#include <iostream>
#include <stdio.h>
#include <chunk.hpp>
#include <metrics.hpp>
#include <circular_buffer.hpp>
#include <thread>
#include <atomic>

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

using IntBuffer = copier::CircularBuffer<copier::Chunk, 100>;

void read_loop(IntBuffer& buffer, FILE* f, std::atomic_bool& eof_reached) {
    for(auto location = 0u;; location++) {
        const auto& c = buffer.write(copier::read_chunk(f, location));
        if(c.last) {
            eof_reached.store(true);
            return;
        }
    }
}

void write_loop(IntBuffer& buffer, FILE* f, copier::Metrics& metrics, std::atomic_bool& eof_reached) {
    while(!eof_reached.load() || !buffer.is_empty()) {
        auto& c = buffer.peek();
        // TODO: error handling
        copier::write_chunk(f, c);
        metrics.processed(c.size);
        buffer.pop();
    }
}

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);
    std::cout << "Copying from " << args.input_path << " to " << args.output_path << '\n';
    copier::Metrics metrics{};
    //TODO: ensure proper file closing. Use fstream instead?
    auto f = fopen(args.input_path.c_str(), "rb");
    auto f_out = fopen(args.output_path.c_str(), "wb");
    IntBuffer intermediate_buffer;

    std::atomic_bool eof_reached{false};

    std::thread read_thread([&]{read_loop(intermediate_buffer, f, eof_reached);});
    std::thread write_thread([&]{write_loop(intermediate_buffer, f_out, metrics, eof_reached);});

    read_thread.join();
    write_thread.join();

    fclose(f);
    fclose(f_out);
    metrics.stop();
    std::cout << "Copied " << metrics.total_size << " bytes in " << metrics.total_ms << " ms with average speed " << metrics.bandwidth_MB() << " MB/s \n";
    return 0;
}

