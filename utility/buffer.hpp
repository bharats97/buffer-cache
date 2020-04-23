#ifndef _BUFFER_HPP_

    #define _BUFFER_HPP_

    #include <cassert>
    #include <iomanip>
    #include <list>
    #include <ostream>

    // Buffer namespace, to differentiate Buffer utilities from other
    // buffer related classes/frameworks of the language.
    namespace buffer {

        // Buffer header class
        class header {

        private:

            // Header flags structure
            struct flags {

                bool locked, valid, delayed_write, read_write, in_demand, old;

                inline flags() : locked(false), valid(true),
                                 delayed_write(false), read_write(false),
                                 in_demand(false), old(false) {}

            };

        public:

            // Basic members of buffer header
            int device_num, block_num, data;

            flags status;

            // Free list iterator
            std::list<header*>::iterator free_list_iterator;

            // Default constructor
            inline header() : device_num(-1), block_num(-1), data(-1),
                              status(), free_list_iterator(nullptr) {}

            // Constructor for block and device number
            inline header(int _device_num, int _block_num) : header() {
                device_num = _device_num;
                block_num = _block_num;
            }

            // Output Stream operator overloaded for printing and debugging
            inline friend std::ostream& operator << (std::ostream &out, const header &buf) {
                out << std::setw(8) << buf.device_num;
                out << std::setw(10) << buf.block_num;
                out << std::setw(14) << (buf.status.locked ? " Locked" : " Free");
                out << std::setw(12) << (buf.status.delayed_write ? " DW" : "");
                out << std::endl;
                return out;
            }

        };

        // Buffer Header Comparator Structure
        struct header_comparator {

            inline bool operator () (const header* const &x, const header* const &y) {
                assert(x != nullptr && y != nullptr);
                if (x->device_num == y->device_num) {
                    return (x->block_num < y->block_num);
                }
                return (x->device_num < y->device_num);
            }

        };

        // Buffer Iterator Structure (a pointer wrapper object)
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
