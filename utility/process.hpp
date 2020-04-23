#ifndef _PROCESS_HPP_

    #define _PROCESS_HPP_

    #include <cstdlib>
    #include <fstream>
    #include <iostream>
    #include <random>

    #include "buffer_cache.hpp"

    // Process function, which denotes a process in memory
    void process(buffer_cache *cache, int pid) {

        // Trying to open up a new terminal for each process
        if (system("gnome-terminal") != 0) {
            // If it returns a non-zero integer, it means 'failure' during
            // execution. So, this process exits (terminates) abnormally.
            std::cerr << "Process Invocation Failed!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Redirect output of each process to a new Terminal
        std::ofstream out(std::string("/dev/pts/") + static_cast<char>(pid + '0'), std::ios_base::out);

        // True (machine) random number generator
        std::random_device rand;

        constexpr const int TOTAL_BLOCKS = 3;
        constexpr const int TOTAL_DEVICES = 1;

        // Number of blocks required by this process
        const int BLOCK_REQUIRED = 1 + rand() % TOTAL_BLOCKS;

        out << "Block Required :- " << BLOCK_REQUIRED << std::endl;

        for (int i = 0; i < BLOCK_REQUIRED; i++) {

            // Device and Block Numbers chosen randomly
            int blk_no = rand() % TOTAL_BLOCKS;
            int dev_no = rand() % TOTAL_DEVICES;

            // 'getblk' algorithm performed to acquire a buffer
            buffer::header *required_buffer = cache->getblk(dev_no, blk_no, out);

            out << "Process with ID: " << pid << " acquired " << dev_no << ", " << blk_no << std::endl;

            // Randomly marks 'delayed_write' to a buffer
            if (!((required_buffer->status).delayed_write)) {
                (required_buffer->status).delayed_write = (rand() % 2 == 0) ? true : false;
            }

            out << *cache << std::endl;
            out << "--------------------------------------------------------------------------" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(3000));

            // Releases the buffer using 'brelse' algorithm
            cache->brelse(required_buffer);
            out << "Process with ID: " << pid << " released " << dev_no << " " << blk_no << std::endl;
            out << *cache << std::endl;
            out << "--------------------------------------------------------------------------" << std::endl;
        }
        out << "Process with pid " << pid << " terminates" << std::endl;
        out.close();
    }

#endif
