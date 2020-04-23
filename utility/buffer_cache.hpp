#ifndef _BUFFER_CACHE_

    #define _BUFFER_CACHE_

    #include <cassert>
    #include <iomanip>
    #include <mutex>
    #include <ostream>
    #include <random>
    #include <set>
    #include <thread>

    #include "buffer.hpp"
    #include "free_list.hpp"

    // The Buffer Cache Class
    class buffer_cache {

    private:

        // Data Members: free list, buffer pool, mutex lock for critical
        // sections, and a random number generator

        free_list fl;

        std::mutex mutex_lock;
        std::random_device rng;

        /*
         * This is the key change that we've done here. We do not maintain a
         * Buffer Hash Array, instead we've used a Balanced Binary Search Tree
         * for fast searching purposes. Complete mathematical analysis about
         * speed and performance boost up, trade offs, etc. have been provided
         * along with this project in a separate PDF file.
         *
         * We've used a reliable data structure from GNU C++ STL. It is 'std::set',
         * which is an implicitly defined RB-Tree (Balanced BST) which keeps
         * unique elements, and keeps them sorted at all times, so all
         * operations work in logarithmic time complexity
         * (in terms of current size of buffer pool).
         */
        std::set<buffer::header*, buffer::header_comparator> pool;

        // Searches a buffer in buffer pool, returns buffer pointer if found,
        // else return 'nullptr'
        // Work in O(log2(pool_size))
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

        // Dummy method to read data from disk (just to show).
        inline int read_data_from_disk(int device_num, int block_num) {
            int duration = 1000 + rng() % 3001;
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            return (device_num ^ block_num);
        }

        // Asynchronous write to disk, for buffers marked with
        // delayed_write flag.
        inline void async_write(buffer::header *buf) {
            int duration = 1000 + rng() % 3001;
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            (buf->status).delayed_write = false;
            (buf->status).old = true;
            brelse(buf);
        }

    public:

        // Constructor
        inline buffer_cache(int size) : fl(size) {}

        // The 'getblk' algorithm, each loop iteration works in O(log2(pool_size)),
        // as compared to the traditional time complexity which is O(pool_size) per
        // iteration.
        inline buffer::header* getblk(int device_num, int block_num, std::ostream &out) {
            buffer::header *buf = nullptr;
            do {
                mutex_lock.lock();
                buf = search(device_num, block_num);
                mutex_lock.unlock();

                if (buf != nullptr) {
                    if ((buf->status).locked) {
                        out << "Case 5: Required Block is in Cache but is Busy" << std::endl;
                        while ((buf->status).locked) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        }
                        out << "Waking up on event(Buffer Becomes Free)" << std::endl;
                        buf = nullptr;
                        continue;
                    }
                    out << "Case 1: Required Block is in Cache and is Free" << std::endl;
                    (buf->status).locked = true;
                    fl.remove(buf);
                    return buf;
                } else {
                    if (fl.empty()) {
                        out << "Case 4: Block is not in Cache and Free List is Empty" << std::endl;
                        while (fl.empty()) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        }
                        out << "Waking up on event(Buffer Becomes Free)" << std::endl;
                        continue;
                    }
                    fl.remove(buf);
                    if ((buf->status).delayed_write) {
                        out << "Case 3: Buffer on Free List was marked Delayed Write" << std::endl;
                        std::thread th(&buffer_cache::async_write, this, buf);
                        th.join();
                        buf = nullptr;
                        continue;
                    }
                    mutex_lock.lock();
                    out << "Case 2: Block is not in Cache so it allocates a buffer from Free List" << std::endl;
                    (buf->status).locked = true;
                    if (buf->device_num != -1 && buf->block_num != -1) {
                        pool.erase(buf);
                    }
                    buf->device_num = device_num;
                    buf->block_num = block_num;
                    buf->data = read_data_from_disk(device_num, block_num);
                    pool.insert(buf);
                    mutex_lock.unlock();
                    return buf;
                }
            } while (buf == nullptr);
            return buf;
        }

        // The Buffer Release algorithm 'brelse'
        inline void brelse(buffer::header* &buf) {
            mutex_lock.lock();
            if ((buf->status).valid && !(buf->status).old) {
                fl.insert_at_tail(buf);
            } else {
                (buf->status).old = false;
                fl.insert_at_head(buf);
            }
            mutex_lock.unlock();
        }

        // Output Stream operator overloaded for printing and debugging
        inline friend std::ostream& operator << (std::ostream &out, const buffer_cache &bc) {
            out << std::endl << "Buffer Pool :";
            out << std::endl << "SNO.";
            out << std::setw(10) << "DEVICE";
            out << std::setw(9) << "BLOCK";
            out << std::setw(15) << "LOCKED/FREE";
            out << std::setw(17) << "DELAYED WRITE";
            int i = 1;
            for (const buffer::header* const &buf : bc.pool) {
                out << std::endl << i << ".   ";
                out << *buf;
                i++;
            }
            out << std::endl << "Free List : " << bc.fl;
            return out;
        }

    };

#endif
