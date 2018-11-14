#include <omp.h>
#include <dirent.h>

#include "Extensions.h"

// Logging
#include "plog/Log.h"

#include "multire.h"


#define MATCHALL match::list | match::node | match::world


std::string GetStdoutFromCommand(std::string cmd) {

    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
    while (!feof(stream))
        if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
    pclose(stream);
    }
    return data;
}



bool Extensions::Collect(int argc, char *argv[])
{
    bool is_ok = true;

    LOG_VERBOSE << "Parsing " << this->Name() << " info...";
    
    std::string path(argv[0]);
    path += "/extensions/";
    
    std::vector<std::string> filelist;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
    /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            std::string entry(ent->d_name);
            if(entry != "." and entry != "..") filelist.push_back(path + ent->d_name);
        }
    closedir (dir);
    } 
    else {
        return false;               
    }
    
    
    std::vector<std::string> failed_extensions;
    
    
    for(std::vector<std::string>::iterator it=filelist.begin(); it < filelist.end() ; ++it){
        std::string output=GetStdoutFromCommand(*it);
        std::vector<std::string> name=DoRegexMatch(output, "Name:[ \t]+(.*)");
        std::vector<std::string> type=DoRegexMatch(output, "Type:[ \t]+(.*)");
        std::vector<std::string> value=DoRegexMatch(output, "Value:[ \t]+(.*)");
        std::vector<std::string> unit=DoRegexMatch(output, "Unit:[ \t]+(.*)");
        std::vector<std::string> message=DoRegexMatch(output, "Message:[ \t]+(.*)");
        
        if(message.size()>0) LOG_VERBOSE << "Extension " << *it << " says: " << message[0];

        
        if(name.size()==0 or type.size()==0 or value.size()==0){
                failed_extensions.push_back(*it);            
                continue;
        }
        
        if(unit.size()==0) unit.push_back("");
        
        if(type[0]=="string" or type[0]=="s"){
            Item<std::string> i = Item<std::string>(name[0], MATCHALL, value);
            m_items.strings.push_back(i);      
        }
        else if(type[0]=="float" or type[0]=="f"){
            
            std::vector<float> fvalue;
            for(std::vector<std::string>::iterator valit=value.begin(); valit < value.end() ; ++valit)  
                fvalue.push_back(std::stof(*valit));
            Item<float> i = Item<float>(name[0], unit[0], MATCHALL, fvalue);
            m_items.floats.push_back(i);      
        }
        else if(type[0]=="integer" or type[0]=="i"){
            
            std::vector<int> fvalue;
            for(std::vector<std::string>::iterator valit=value.begin(); valit < value.end() ; ++valit)  
                fvalue.push_back(std::stof(*valit));
            Item<int> i = Item<int>(name[0], unit[0], MATCHALL, fvalue);
            m_items.integers.push_back(i);      
        }
        else{
            failed_extensions.push_back(*it);            
        }
        }
        
        if(failed_extensions.size()>0){
            
            Item<std::string> i = Item<std::string>("Failed", MATCHALL, failed_extensions);
            m_items.strings.push_back(i);   
            
        }
    


    return is_ok;
};
