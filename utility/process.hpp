#pragma once

#include <iostream>
#include <chrono>
#include <fstream>
#include <string.h>
#include "buffer_cache.hpp"


void process(buffer_cache *cache,int pid)
{
	std::random_device rand;
	//Redirect output of each thread to a new Terminal
	std::string s="/dev/pts/"+std::string(1,char(pid+'0')); 
	system("gnome-terminal");
	std::ofstream cout(s, std::ios_base::out);
	
	int TOTAL_BLOCKS=3;
	int TOTAL_DEVICES=1;
	int BLOCK_REQUIRED=rand()%TOTAL_BLOCKS+1;

	cout<<"Block required :- "<<BLOCK_REQUIRED<<std::endl;

	for(int i=0;i<BLOCK_REQUIRED;i++)
	{
		int blk_no=rand()%TOTAL_BLOCKS;
		int dev_no=rand()%TOTAL_DEVICES;

		buffer::header *required_buffer=cache->getblk(dev_no,blk_no,cout);
		cout<<"Process with ID: "<<pid<<" acquired "<<dev_no<<","<<blk_no<<std::endl;
		if((required_buffer->status).delayed_write)
			(required_buffer->status).delayed_write=true;
		else
			(required_buffer->status).delayed_write=(rand()%2==0)?true:false;
		cout<<*cache<<std::endl;
		cout<<"--------------------------------------------------------------------------"<<std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		
		cache->brelse(required_buffer);
		cout<<"Process with ID: "<<pid<<" released "<<dev_no<<" "<<blk_no<<std::endl;
		cout<<*cache<<std::endl;
		cout<<"--------------------------------------------------------------------------"<<std::endl;
	}
	cout<<"Thread with pid "<<pid<<" terminates "<<std::endl;

}
