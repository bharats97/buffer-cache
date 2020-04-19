#ifndef _BUFFER_CACHE_

    #define _BUFFER_CACHE_
    #include <cassert>
    #include <fstream>
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

        inline buffer::header* getblk(int device_num, int block_num,std::ofstream &cout) {
            buffer::header *buf = nullptr;
            do {
                mutex_lock.lock();
                buf = search(device_num, block_num);
                mutex_lock.unlock();
          
                if (buf != nullptr) {
                    if ((buf->status).locked) {
                        cout<<"Case 5: Required Block is in cache but is Busy\n";
                        while((buf->status).locked)
                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        cout<<"Waking up on event(Buffer Becomes Free) \n";
                        buf = nullptr;
                        continue;
                    }
                    cout<<"Case 1: Required Block is in cache and is free \n";
                    (buf->status).locked = true;
                    fl.remove(buf);
                    return buf;
                } else {
                    if (fl.empty()) {
                        cout<<"Case 4: Block is not in cache and Free List is empty \n";
                        while(fl.empty())
                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        cout<<"Waking up on event(Buffer Becomes Free) \n";
                        continue;
                    }
                    fl.remove(buf);
                    if ((buf->status).delayed_write) {
                        cout<<"Case 3: Buffer on free list was marked Delayed Write\n";
                        std::thread th(&buffer_cache::async_write, this, buf);
                        th.join();
                        buf = nullptr;
                        continue;
                    }
                    mutex_lock.lock();
                    cout<<"Case 2: Block is not in cache so it allocates a buffer from Free List \n";
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

        inline friend std::ostream & operator << (std::ostream &output, const buffer_cache &bc) {
            int i = 1;
            output << "\nBuffer Pool :";
            output << "\nSNO."; 
            output << std::setw(10) << "DEVICE" ;
            output << std::setw(9) << "BLOCK" ;
            output << std::setw(15) << "LOCKED/FREE" ; 
            output << std::setw(17) << "DELAYED WRITE";
            for (auto buf : bc.pool) {
                output << "\n" <<i<<".   ";
                output << *buf;
                i++;
            }

            output << "\nFree List : " << bc.fl;
            return output;

        }
    };

#endif
