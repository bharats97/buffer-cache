#ifndef _FREE_LIST_HPP_

    #define _FREE_LIST_HPP_
    
    #include <list>
    
    #include "utility/buffer.hpp"
    
    struct free_list {
        
        std::list<buffer::header*> lst;
        
        inline free_list(int n) : lst(n) {}
        
    };

#endif
