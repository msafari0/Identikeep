#include <iostream>
#include <fstream>
#include "Utils.h"
#include "multire.h"
#include "Mem.h"

// Logging
#include "plog/Log.h"

#define MEM_INFO_FILE "/proc/meminfo"


bool MEM::Collect(int argc, char *argv[])
{
    bool is_ok=true;
    LOG_INFO << "Parsing " <<  this->Name() << " info...";
    
    /* Open /proc/meminfo for reading. This should be perfectly fine on linux */
    std::ifstream ifs(MEM_INFO_FILE);
    if (ifs.fail()) {
        LOG_ERROR << "Could not open " MEM_INFO_FILE;
        return false;
    }

    /* parse all content into cpu_info string, used later for regex matching */
    std::string mem_info( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );  
    
    
    /*  == Get Total Memory, kB ==  */
    std::vector<std::string> res = DoRegexMatch(mem_info, "MemTota[l|l ]:[ \t]+([0-9]+)[ \t]*kB");

    LOG_DEBUG << "Total memory parsing got " << res.size() << " elements.";
    
    if (res.size() > 0) {
        int msize = 0;
        /* Only one entry in /proc/meminfo for Total memory, 
         * try to convert it to int */
        try {
            if (res.size() == 1) {
                msize = std::stoi(res[0]);
            } else { 
                LOG_ERROR << "Invalid number of elements in Total memory: " << res.size() <<  " elements.";
                is_ok = false;
            }
        } catch (const std::exception &e) { LOG_ERROR << "Error in MEM::Collect:" <<  e.what(); is_ok = false; }
        
        /* Only add value if valid */
        if ( msize > 0 ) {
            Item<int> i = Item<int>("memTotal", "kB", match::none, msize);
            m_items.integers.push_back(i);
        }
        
    } else {
        LOG_ERROR << "Could not parse total memory.";
        is_ok = false;
    }
    return is_ok;

};
