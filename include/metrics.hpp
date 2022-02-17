
#include <chrono>

namespace copier{
    class Metrics {
    public:
        Metrics() { 
            start();
        }
        void start();
        void stop();
        void processed(unsigned int bytes);
        double bandwidth_MB() const;

        unsigned int total_size {0};
        double total_ms {0};

    private:
        std::chrono::high_resolution_clock::time_point start_ts;
    };
}