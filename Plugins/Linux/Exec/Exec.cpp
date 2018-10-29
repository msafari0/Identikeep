#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <time.h>

#include "Utils.h"
// #include "multire.h"
#include "Exec.h"



// Logging
#include "plog/Log.h"

// #define _INFO_FILE "/proc/version"
#define MATCHALL match::list | match::node | match::world
#define PATH_MAX 1024




std::string getCmdOutput(const std::string& mStr)
{
    std::string result, file;
    FILE* pipe{popen(mStr.c_str(), "r")};
    char buffer[PATH_MAX];

    while(fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        file = buffer;
        result += file.substr(0, file.size() - 1);
    }

    pclose(pipe);
    return result;
}


std::string do_readlink(std::string const& path) {
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
    /* handle error condition */
}

int fileSize(const char *add){
    std::ifstream mySource;
    mySource.open(add, std::ios_base::binary);
    mySource.seekg(0,std::ios_base::end);
    int size = mySource.tellg();
    mySource.close();
    return size;
}


std::string getCreationDate(std::string filename){
    struct tm* clock;               // create a time structure
    struct stat attrib;         // create a file attribute structure
    stat(filename.c_str(), &attrib);     // get the attributes of afile.txt
    clock = gmtime(&(attrib.st_mtime)); 
    std::stringstream ss;
    ss << clock->tm_mon+1 << "-" << clock->tm_mday <<"-"<< clock->tm_year+1900 <<" at "<< clock->tm_hour <<":"<< clock->tm_min;
    return ss.str();
}


bool Exec::Collect(int argc, char *argv[])
{
    bool is_ok=true;
    LOG_INFO << "Parsing " << this->Name() <<  " info...";
  
    std::string proc_pid=std::to_string(getpid());
    std::string filename="/proc/"+proc_pid+"/exe";
    
    std::string path=do_readlink(filename);
    

    Item<std::string> info_path = Item<std::string>("execPath", MATCHALL, path);
    m_items.strings.push_back(info_path);
    
    ////////////////////////////////
    
    int exec_size=fileSize(path.c_str());
    Item<int> info_exec_size = Item<int>("execSize", "Bytes",  MATCHALL, exec_size);
    m_items.integers.push_back(info_exec_size);
    
    
    //////////////////////////////////////
    
    Item<std::string> info_date = Item<std::string>("execLastModified", MATCHALL,getCreationDate(path) );
    m_items.strings.push_back(info_date);    
    
    
    
    
    
    
    return is_ok;
};
