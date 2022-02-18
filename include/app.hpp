#pragma once

#include <stdio.h>
#include <metrics.hpp>
#include <circular_buffer.hpp>
#include <atomic>
#include <chunk.hpp>


namespace app {

    // Main application logic.
    // Runs 2 threads - one for reading from source and the other for writing to destination.
    // Data between those threads is passed as chunks through intermediate buffer.
    class App {
        // Using intermediate buffer with size 100 (first value that came to mind). This might be subject to optimisation.
        using IntermediateBuffer = copier::CircularBuffer<copier::Chunk, 100>;

        public:
            void run(FILE* src, FILE* dst, copier::Metrics& metrics);

        private:
            void read_loop(FILE* src);
            void write_loop(FILE* dst, copier::Metrics& metrics);

            std::atomic_bool eof_reached{false};
            IntermediateBuffer buffer;
    };
}
