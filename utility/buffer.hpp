#ifndef _BUFFER_HPP_

    #define _BUFFER_HPP_

    #include <cassert>

    namespace buffer {

        struct flags {

            bool locked, valid, delayed_write, read_write, in_demand;

            inline flags() : locked(false), valid(true), delayed_write(false),
                             read_write(false), in_demand(false) {}

        };

        struct header {

            int device_num, block_num, data;

            flags status;

            header *next_free, *prev_free;

            inline header() : device_num(-1), block_num(-1), data(-1), status(),
                              next_free(nullptr), prev_free(nullptr) {}

            inline ~header() {
                if (next_free != nullptr) {
                    delete[] next_free;
                }
                if (prev_free != nullptr) {
                    delete[] prev_free;
                }
            }

        };

        struct header_comparator {

            inline bool operator () (const header *x, const header *y) {
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