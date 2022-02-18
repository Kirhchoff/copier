#include <thread>
#include <app.hpp>


namespace app {

void App::read_loop(FILE* src) {
    for(auto location = 0u;; location++) {
        const auto& c = buffer.write(copier::read_chunk(src, location));
        if(c.last) {
            eof_reached.store(true);
            return;
        }
    }
}

void App::write_loop(FILE* dst, copier::Metrics& metrics) {
    while(!eof_reached.load() || !buffer.is_empty()) {
        auto& c = buffer.peek();
        copier::write_chunk(dst, c);
        metrics.processed(c.size);
        buffer.pop();
    }
}

void App::run(FILE* src, FILE* dst, copier::Metrics& metrics) {
    eof_reached.store(false);
    std::thread read_thread([&]{read_loop(src);});
    std::thread write_thread([&]{write_loop(dst, metrics);});
    read_thread.join();
    write_thread.join();
}
}
