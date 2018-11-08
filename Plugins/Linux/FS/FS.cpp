#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "multire.h"

#include "FS.h"

// Logging
#include "plog/Log.h"


#define COMMAND "stat -f ."
#define BUFFERSIZE 256
#define MATCHALL match::list | match::node | match::world

bool FS::Collect(int argc, char *argv[])
{
    bool is_ok=true;
    LOG_VERBOSE << "Parsing " << this->Name() <<  " info...";
    
    std::string command(COMMAND);

    std::array<char, BUFFERSIZE> buffer;
    std::string result;

    LOG_VERBOSE << "Opening reading pipe";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        LOG_ERROR <<  "Couldn't start command " << COMMAND;
        return 0;
    }
    while (fgets(buffer.data(), BUFFERSIZE, pipe) != NULL) {
        result += buffer.data();
    }
    auto returnCode = pclose(pipe);


    std::vector<std::string> FStype = DoRegexMatch(result, "[tT]ype:[ \t]+(.*)");
    std::vector<std::string> FSBlockSize = DoRegexMatch(result, "Block size:[ \t]+([0-9]*)");
    
    if(FStype.size()>0){    
            Item<std::string> i = Item<std::string>("fsType", MATCHALL , FStype[0]);
            m_items.strings.push_back(i);
    }
    if(FStype.size()>0){    
            Item<int> i = Item<int>("fsBlockSize", "Byte",  MATCHALL , std::stoi(FSBlockSize[0]));
            m_items.integers.push_back(i);
    }
    return is_ok;
};
