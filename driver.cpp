#include <iostream>
#include <dirent.h>

#include "utility/buffer_cache.hpp"
#include "utility/process.hpp"

int main() {
    // Initialising a Buffer Cache object with free list of size 2
    // and an empty buffer pool
    buffer_cache cache(2);
    DIR *dp = opendir("/dev/pts/");
    int i = 0;
    if (dp != nullptr) {
        while (readdir(dp)) {
            i++;
        }
        closedir(dp);
    } else {
        std::cerr << "Couldn't open the Directory" << std::endl;
    }

    std::thread t1(process, &cache, i - 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::thread t2(process, &cache, i - 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::thread t3(process, &cache, i - 1);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
