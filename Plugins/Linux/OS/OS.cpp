#include <regex>
#include <iostream>
#include <fstream>
#include "OS.h"

// Logging
#include "plog/Log.h"

#define OS_INFO_FILE "/proc/version"



bool OS::Collect(int argc, char *argv[])
{
    bool is_ok=true;
    LOG_VERBOSE << "Parsing " << this->Name() <<  " info...";
    
    /* Open /proc/meminfo for reading. This should be perfectly fine on linux */
    std::ifstream ifs(OS_INFO_FILE);
    if (ifs.fail()) {
        LOG_ERROR << "Could not open " OS_INFO_FILE;
        return false;
    }

    /* parse all content into cpu_info string, used later for regex matching */
    std::string os_info( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );  
    LOG_DEBUG << "OS version: " << os_info;
    Item<std::string> i = Item<std::string>("osVersion", match::list | match::node | match::world, os_info);
    m_items.strings.push_back(i);

    return is_ok;
};
