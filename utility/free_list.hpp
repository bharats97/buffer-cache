#ifndef _FREE_LIST_HPP_

    #define _FREE_LIST_HPP_

    #include <cassert>
    #include <iterator>
    #include <list>
    #include <mutex>
    #include "buffer.hpp"

    class free_list {

    private:

        unsigned int capacity;

        std::list<buffer::header*> lst;
        std::mutex mutex_lock;

        inline void clear_iterator(std::list<buffer::header*>::iterator &it) {
            it = std::list<buffer::header*>::iterator(nullptr);
        }

    public:

        inline free_list(int n) {
            assert(n > 0);
            capacity = n;
            for (unsigned int i = 0; i < capacity; i++) {
                lst.push_back(new buffer::header());
                lst.back()->free_list_iterator = std::prev(lst.end());
            }
        }

        inline ~free_list() {
            for (buffer::header* &buf : lst) {
                delete[] buf;
                buf = nullptr;
            }
        }

        inline void insert_at_head(buffer::header* &buf) {
            mutex_lock.lock();
            assert(buf != nullptr);
            lst.push_front(buf);
            buf->free_list_iterator = lst.begin();
            (buf->status).locked = false;
            mutex_lock.unlock();
        }

        inline void insert_at_tail(buffer::header* &buf) {
            mutex_lock.lock();
            assert(buf != nullptr);
            lst.push_back(buf);
            buf->free_list_iterator = std::prev(lst.end());
            (buf->status).locked = false;
            mutex_lock.unlock();
        }

        inline void remove(buffer::header* &buf) {
            mutex_lock.lock();
            if (buf == nullptr) {
                buf = lst.front();
            }
            lst.erase(buf->free_list_iterator);
            clear_iterator(buf->free_list_iterator);
            mutex_lock.unlock();
        }

        inline bool empty() {
            return lst.empty();
        }

        inline friend std::ostream & operator << (std::ostream &output, const free_list &fl) {
            int i = 1;
            output << "\nSNO."; 
            output << std::setw(10) << "DEVICE" ;
            output << std::setw(9) << "BLOCK" ;
            output << std::setw(15) << "LOCKED/FREE" ; 
            output << std::setw(17) << "DELAYED WRITE";
            for (auto buf : fl.lst) {
                output << "\n" << i<<". ";
                output << *buf;
                i++;
            }
            output << "\n";
            return output;
        }
    };

#endif
