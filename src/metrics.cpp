
#include <chrono>
#include <metrics.hpp>

namespace copier{
    void Metrics::start() {
        start_ts = std::chrono::high_resolution_clock::now();
    }
    void Metrics::stop() {
        auto stop_ts = std::chrono::high_resolution_clock::now();
        total_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(stop_ts - start_ts).count();
    }
    void Metrics::processed(unsigned int bytes) {
        total_size += bytes;
    }
    double Metrics::bandwidth_MB() const {
        return total_size/total_ms * 1000 / 1048576;
    }
}