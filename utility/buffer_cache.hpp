#ifndef _BUFFER_CACHE_

    #define _BUFFER_CACHE_

    #include <cassert>
    #include <mutex>
    #include <random>
    #include <set>
    #include <thread>

    #include "buffer.hpp"
    #include "free_list.hpp"

    class buffer_cache {

    private:

        free_list fl;

        std::mutex mutex_lock;
        std::random_device rng;
        std::set<buffer::header*, buffer::header_comparator> pool;

        inline buffer::header* search(int device_num, int block_num) {
            assert(device_num >= 0 && block_num >= 0);
            buffer::header *dummy = new buffer::header(device_num, block_num);
            buffer::header *buf = nullptr;
            std::set<buffer::header*>::iterator it = pool.find(dummy);
            if (it != pool.end()) {
                buf = *it;
            }
            delete[] dummy;
            return buf;
        }

        inline int read_data_from_disk(int device_num, int block_num) {
            int duration = 1000 + rng() % 3001;
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            return (device_num ^ block_num);
        }

        inline void async_write(buffer::header *buf) {
            int duration = 1000 + rng() % 3001;
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            (buf->status).delayed_write = false;
            (buf->status).old = true;
            brelse(buf);
        }

    public:

        inline buffer_cache(int size) : fl(size) {}

        inline buffer::header* getblk(int device_num, int block_num) {
            buffer::header *buf = nullptr;
            do {
                buf = search(device_num, block_num);
                if (buf != nullptr) {
                    if ((buf->status).locked) {
                        // TODO: sleep (event buffer becomes free)
                        buf = nullptr;
                        continue;
                    }
                    (buf->status).locked = true;
                    fl.remove(buf);
                    return buf;
                } else {
                    if (fl.empty()) {
                        // TODO: sleep (event any buffer becomes free)
                        continue;
                    }
                    fl.remove(buf);
                    if ((buf->status).delayed_write) {
                        std::thread th(&buffer_cache::async_write, this, buf);
                        th.detach();
                        buf = nullptr;
                        continue;
                    }
                    (buf->status).locked = true;
                    if (buf->device_num != -1 && buf->block_num != -1) {
                        pool.erase(buf);
                    }
                    buf->device_num = device_num;
                    buf->block_num = block_num;
                    buf->data = read_data_from_disk(device_num, block_num);
                    pool.insert(buf);
                    return buf;
                }
            } while (buf == nullptr);
        }

        inline void brelse(buffer::header* &buf) {
            // TODO: event wake up for specific and any buffer
            mutex_lock.lock();
            if ((buf->status).valid && !(buf->status).old) {
                fl.insert_at_tail(buf);
            } else {
                (buf->status).old = false;
                fl.insert_at_head(buf);
            }
            mutex_lock.unlock();
        }

    };

#endif
