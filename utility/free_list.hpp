#ifndef _FREE_LIST_HPP_

    #define _FREE_LIST_HPP_
    
    #include <list>
    
    #include <cassert>

    #include "buffer.hpp"
    
    struct free_list {
        
        std::list<buffer::header> lst;

        long unsigned int capacity;
        
        inline free_list (int n) : capacity(n){}

        inline std::list<buffer::header>::iterator head () {
        	assert(!lst.empty());
        	return lst.begin ();
        }

        std::list<buffer::header>::iterator add(buffer::header buf) {
        	assert((lst.size() != capacity) && !buf.status.locked);
        	lst.push_back(buf);
        	buf.status.locked = false;
        	std::list<buffer::header>::iterator it = lst.end();
        	it--;
        	return it;
        }

        buffer::header remove(std::list<buffer::header>::iterator it) {
        	buffer::header buf = *it;
        	lst.erase(it);
        	return buf;
        }
        
    };

#endif