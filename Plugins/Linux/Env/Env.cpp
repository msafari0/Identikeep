#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/utsname.h>
#include "Utils.h"
// #include "multire.h"
#include "Env.h"

// Logging
#include "plog/Log.h"

// #define _INFO_FILE "/proc/version"
#define MATCHALL match::list | match::node | match::world

extern char **environ;

bool Env::Collect(int argc, char *argv[])
{
    bool is_ok=true;
    LOG_VERBOSE << "Parsing " << this->Name() <<  " info...";
    std::vector<std::string> env_list;

    
    for(char **current = environ; *current; current++) {
        env_list.push_back(*current);
    }
    
 
    Item<std::string> ii = Item<std::string>("envVariables", MATCHALL, env_list);
    m_items.strings.push_back(ii);
    
    
    return is_ok;
};
