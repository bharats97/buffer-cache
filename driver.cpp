#include <iostream>
#include <chrono>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include "utility/process.hpp"
#include "utility/buffer_cache.hpp"

int main()
{
	//Initialising a buffer cache object with free list of size 2 and an empty buffer pool
	buffer_cache cache(2);
	DIR *dp;
  	int i = 0;
	dp = opendir ("/dev/pts/");
  	if (dp != NULL)
  	{
    	while (readdir(dp))
      	i++;

    	closedir (dp);
  	}
  	else
    	std::cout<<"Couldn't open the directory"<<std::endl;

	std::thread t1(process,&cache,i-3);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::thread t2(process,&cache,i-2);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::thread t3(process,&cache,i-1);

	t1.join();
	t2.join();
	t3.join();
	return 0;
}
