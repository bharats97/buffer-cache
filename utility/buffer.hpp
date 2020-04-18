#ifndef _BUFFER_HPP_

    #define _BUFFER_HPP_

    #include <cassert>
    #include <list>
    #include <ostream>
    #include <iomanip>

    namespace buffer {

        class header {

        private:

            struct flags {

                bool locked, valid, delayed_write, read_write, in_demand, old;

                inline flags() : locked(false), valid(true),
                                 delayed_write(false), read_write(false),
                                 in_demand(false), old(false) {}

            };

        public:

            int device_num, block_num, data;

            flags status;

            std::list<header*>::iterator free_list_iterator;

            inline header() : device_num(-1), block_num(-1), data(-1),
                              status(), free_list_iterator(nullptr) {}

            inline header(int _device_num, int _block_num) : header() {
                device_num = _device_num;
                block_num = _block_num;
            }

            inline friend std::ostream & operator << (std::ostream &output, const header &buf) {
                output << std::setw(8) << buf.device_num; 
                output << std::setw(10) << buf.block_num;
                output << std::setw(14) << (buf.status.locked ? " Locked" : " Free");
                output << std::setw(12) << (buf.status.delayed_write ? " DW" : "");
                output << "\n";
                return output;
            }

        };

        struct header_comparator {

            inline bool operator () (const header* const &x, const header* const &y) {
                assert(x != nullptr && y != nullptr);
                if (x->device_num == y->device_num) {
                    return (x->block_num < y->block_num);
                }
                return (x->device_num < y->device_num);
            }

        };

        /*struct iterator {

            const header *buf;

            inline iterator(const header *real_buf) {
                assert(real_buf != nullptr);
                buf = real_buf;
            }

            inline bool operator < (const iterator &other) const {
                if (buf->device_num == (other.buf)->device_num) {
                    return (buf->block_num < (other.buf)->block_num);
                }
                return (buf->device_num < (other.buf)->device_num);
            }

        };*/

    }

#endif
