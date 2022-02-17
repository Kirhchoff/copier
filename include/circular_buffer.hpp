#pragma once

#include <condition_variable>
#include <mutex>
#include <iostream>

namespace copier {
    template<class DataPiece, unsigned int SIZE>
    class CircularBuffer {
    public:
        void write(const DataPiece data) {
            std::unique_lock<std::mutex> lock(m);
            data_changed.wait(lock, [this, &data] {return free_space > 0;});
            buffer[write_pos++] = data;
            if(write_pos == SIZE) {
                write_pos = 0;
            }
            if(write_pos == locked_pos) {
                free_space = 0;
            } else if (write_pos > locked_pos) {
                free_space = SIZE - write_pos + locked_pos;
            } else {
                free_space = locked_pos - write_pos;
            }
            data_changed.notify_all();
        }

        const DataPiece& read() {
            std::unique_lock<std::mutex> lock(m);
            data_changed.wait(lock, [this] {return read_pos != write_pos || free_space == 0;});
            const DataPiece& retval = buffer[read_pos++];
            if(read_pos == SIZE) {
                read_pos = 0;
            }
            if(read_pos == locked_pos) {
                std::cout << "Whole buffer read without release or write!\n";
                 // We need to wait for new data here, otherwise we would read the same data again.
                data_changed.wait(lock, [this] {return read_pos != write_pos;});
            }
            data_changed.notify_all();
            return retval;
        }

        void release() {
            std::lock_guard<std::mutex> lock(m);
            locked_pos = read_pos;
            if(write_pos == locked_pos) {
                free_space = SIZE;
            } else if (write_pos > locked_pos) {
                free_space = SIZE - write_pos + locked_pos;
            } else {
                free_space = locked_pos - write_pos;
            }
            data_changed.notify_all();
        }

        bool empty() {
            return free_space == SIZE;
        }

    protected:
        std::array<DataPiece, SIZE> buffer;
        size_t read_pos {}; // position of next read
        size_t write_pos {}; // position of next write
        size_t locked_pos {}; // position "locked" by reader - the reference to data is still in use, so it cannot be overwritten
        size_t free_space {SIZE};

    private:
        std::condition_variable data_changed;
        std::mutex m;
    };
}