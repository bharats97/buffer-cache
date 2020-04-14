#ifndef _BUFFER_CACHE_
	#define _BUFFER_CACHE_
	#include <set>
	#include "buffer.hpp"
	#include "free_list.hpp"
	

	class buffer_cache {
	private:
		std::set<buffer::header,buffer::header_comparator> pool;
		free_list fl;	
	public:
		inline buffer_cache(int size):fl(size) {}

		buffer::header add(int device_num, int block_num, int data) {
			buffer::header buf = fl.remove(fl.head());
			buf.device_num = device_num;
			buf.block_num = block_num;
			buf.data = data;
			buf.status.locked = true;
			buf.status.read_write = true;
			pool.insert(buf);
			return buf;
		}

		void fl_add(buffer::header buf) {
			buf.free_list_iterator = fl.add(buf);
		}

		std::set<buffer::header>::iterator search(int block_num) {
			//way to look for a buffer with specific block_num
		}


	};

#endif
