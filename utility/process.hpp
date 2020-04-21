#ifndef _PROCESS_HPP_

    #define _PROCESS_HPP_

    #include <cstdlib>
    #include <fstream>
    #include <iostream>
    #include <random>

    #include "buffer_cache.hpp"

    void process(buffer_cache *cache, int pid) {
        if (system("gnome-terminal") != 0) {
            std::cerr << "Process Invocation Failed!" << std::endl;
            exit(EXIT_FAILURE);
        }
        // Redirect output of each thread to a new Terminal
        std::ofstream out(std::string("/dev/pts/") + static_cast<char>(pid + '0'), std::ios_base::out);

        std::random_device rand;
        constexpr const int TOTAL_BLOCKS = 3;
        constexpr const int TOTAL_DEVICES = 1;
        const int BLOCK_REQUIRED = 1 + rand() % TOTAL_BLOCKS;

        out << "Block Required :- " << BLOCK_REQUIRED << std::endl;

        for (int i = 0; i < BLOCK_REQUIRED; i++) {
            int blk_no = rand() % TOTAL_BLOCKS;
            int dev_no = rand() % TOTAL_DEVICES;

            buffer::header *required_buffer = cache->getblk(dev_no, blk_no, out);
            out << "Process with ID: " << pid << " acquired " << dev_no << ", " << blk_no << std::endl;
            if ((required_buffer->status).delayed_write) {
                (required_buffer->status).delayed_write = true;
            } else {
                (required_buffer->status).delayed_write = (rand() % 2 == 0) ? true : false;
            }
            out << *cache << std::endl;
            out << "--------------------------------------------------------------------------" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(3000));

            cache->brelse(required_buffer);
            out << "Process with ID: " << pid << " released " << dev_no << " " << blk_no << std::endl;
            out << *cache << std::endl;
            out << "--------------------------------------------------------------------------" << std::endl;
        }
        out << "Thread with pid " << pid << " terminates" << std::endl;
        out.close();
    }

#endif
