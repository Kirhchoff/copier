#pragma once

#include <condition_variable>
#include <mutex>
#include <iostream>

namespace copier {
    // Thread safe FIFO circular buffer class with static storage size.
    // Provided mainly to avoid too much memory allocations compared to using some standard containers with dynamic size (like vector/queue)
    // No constraints for size or data type stored are implementer. Use with care.

    // TODO: Could it be made simpler by using std::deque with custom allocator / pmr?
    // TODO: Does it work with size=1?
    template<class DataPiece, unsigned int SIZE>
    class CircularBuffer {
    public:
        // Write single piece of data. 
        // Will block if there is no space available in the buffer.
        // Retrurns referenc to the data piece written inside the buffer.
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

        // Read a single piece of data from top.
        // Will block if there is no data in the buffer.
        const DataPiece& peek() const {
            std::unique_lock<std::mutex> lock(m);
            // Make sure we are peeking actual data, not some garbage that was never written before
            data_changed.wait(lock, [this] {return !is_empty();});
            return buffer[read_pos];
        }

        // Remove a piece of data from top.
        // Does nothing if buffer is empty.
        void pop() {
            std::lock_guard<std::mutex> lock(m);
            if(is_empty()) {
                return;
            }
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

        // Check if buffer is empty.
        bool is_empty() const {
            return free_space == SIZE;
        }

    protected:
        std::array<DataPiece, SIZE> buffer;
        size_t read_pos {}; // position of next read
        size_t write_pos {}; // position of next write
        size_t free_space {SIZE};

    private:
        mutable std::condition_variable data_changed;
        mutable std::mutex m;
    };
}