#pragma once

#include <chrono>

// Metrics tracking class.
// Tracks execution time (real) and amount of data processed.
namespace copier {
class Metrics {
  public:
    Metrics();

    // Start tracking - starts timer and resets metrics.
    void start();

    // Stop tracking - stops timer
    // Return calculated transfer rate in MB per second
    double stop();

    // Indicate amount of data processed (adds this amount to total)
    void processed(unsigned int bytes);

    unsigned int total_size{0};
    double total_ms{0};

  private:
    // Calculate transfer rate in MB per second
    double rate_MBps() const;

    std::chrono::high_resolution_clock::time_point start_ts;
};
} // namespace copier