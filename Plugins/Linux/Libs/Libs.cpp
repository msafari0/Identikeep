#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/utsname.h>
#include "Utils.h"
// #include "multire.h"
#include "Libs.h"

// Logging
#include "plog/Log.h"

// #define _INFO_FILE "/proc/version"
#define MATCHALL match::list | match::node | match::world


std::string getCmdOutput(const std::string& mStr)
{
    std::string result, file;
    FILE* pipe{popen(mStr.c_str(), "r")};
    char buffer[256];

    while(fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        file = buffer;
        result += file.substr(0, file.size() - 1);
    }

    pclose(pipe);
    return result;
}


bool Libs::Collect(int argc, char *argv[])
{
    bool is_ok=true;
    LOG_INFO << "Parsing " << this->Name() <<  " info...";
//     
//     
//     
//     
    std::string proc_pid=std::to_string(getpid());
    std::string libs_info, filename;
    
    filename="/proc/"+proc_pid+"/maps";
    LOG_INFO << "Reading file " << filename ;
    
    std::ifstream file;
    
    try{
    file.open(filename.c_str());
    } catch(std::ios_base::failure& e) {
        LOG_INFO<< e.what() << '\n';
    }
    
    std::vector<std::string> libs_list;
//     
    int icol=6;
    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string libs;
        for(int i=0; i<icol; i++) iss>>libs;
        
        std::size_t found_string;        
        found_string=libs.find(".so");
        
        if (found_string!=std::string::npos){
                bool found=0;
                for (auto i = libs_list.begin(); i != libs_list.end(); ++i) if(*i==libs) found=1;
                if (!found){ libs_list.push_back(libs); LOG_VERBOSE<< libs;}
            }
    }
    
    
 
    Item<std::string> ii = Item<std::string>("libsLoaded", MATCHALL, libs_list);
    m_items.strings.push_back(ii);
    
    
    return is_ok;
};
