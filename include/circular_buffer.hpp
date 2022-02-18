#pragma once

#include <condition_variable>
#include <mutex>
#include <iostream>

namespace copier {
    // TODO: brief description of the class
    // TODO: Could it be made simpler by using std::deque with custom allocator / pmr?
    // TODO: Does it work with size=1?
    template<class DataPiece, unsigned int SIZE>
    class CircularBuffer {
    public:
        const DataPiece& write(const DataPiece data) {
            std::unique_lock<std::mutex> lock(m);
            // Make sure we don't overwrite data that haven't been read yet
            data_changed.wait(lock, [this, &data] {return free_space > 0;});
            auto written_pos = write_pos;
            // Write data and move next write position
            buffer[write_pos++] = data;
            if(write_pos == SIZE) {
                write_pos = 0;
            }
            // Update free space
            if(write_pos == read_pos) {
                free_space = 0;
            } else if (write_pos > read_pos) {
                free_space = SIZE - write_pos + read_pos;
            } else {
                free_space = read_pos - write_pos;
            }
            data_changed.notify_all();
            return buffer[written_pos];
        }

        const DataPiece& peek() {
            std::unique_lock<std::mutex> lock(m);
            // Make sure we are peeking actual data, not some garbage that was never written before
            data_changed.wait(lock, [this] {return read_pos != write_pos || free_space == 0;});
            return buffer[read_pos];
        }

        void pop() {
            std::lock_guard<std::mutex> lock(m);
            // Move next read position
            read_pos++;
            if(read_pos == SIZE) {
                read_pos = 0;
            }
            // Update free space
            // TODO: smells like code duplication
            if(write_pos == read_pos) {
                free_space = SIZE;
            } else if (write_pos > read_pos) {
                free_space = SIZE - write_pos + read_pos;
            } else {
                free_space = read_pos - write_pos;
            }
            data_changed.notify_all();
        }

        bool is_empty() const {
            return free_space == SIZE;
        }

    protected:
        std::array<DataPiece, SIZE> buffer;
        size_t read_pos {}; // position of next read
        size_t write_pos {}; // position of next write
        size_t free_space {SIZE};

    private:
        std::condition_variable data_changed;
        std::mutex m;
    };
}