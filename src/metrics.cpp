
#include <chrono>
#include <metrics.hpp>

namespace copier{
    Metrics::Metrics() { 
            start();
    }

    void Metrics::start() {
        start_ts = std::chrono::high_resolution_clock::now();
        total_size = 0;
        total_ms = 0;
    }

    double Metrics::stop() {
        auto stop_ts = std::chrono::high_resolution_clock::now();
        total_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(stop_ts - start_ts).count();
        return rate_MBps();
    }

    void Metrics::processed(unsigned int bytes) {
        total_size += bytes;
    }

    double Metrics::rate_MBps() const {
        if (total_ms == 0) {
            return 0;
        }
        return total_size/total_ms * 1000 / 1048576;
    }
}