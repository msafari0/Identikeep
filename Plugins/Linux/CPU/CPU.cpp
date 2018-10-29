#include <iostream>
#include <fstream>
#include "Utils.h"
#include "multire.h"
#include "CPU.h"

#include <sys/utsname.h>


// Logging
#include "plog/Log.h"

#define MATCHALL match::list | match::node | match::world
#define CPU_INFO_FILE "/proc/cpuinfo"
#define CACHE_INFO_DIR "/sys/devices/system/cpu/cpu"
#define CACHE_INFO_FILE "/cache/index"

bool CPU::Collect(int argc, char *argv[])
{
    struct utsname sys_name;
    uname(&sys_name);
    LOG_INFO << sys_name.sysname << sys_name.machine;
    
    if (std::string(sys_name.sysname) != "Linux") {
        LOG_WARNING << "Linux CPU info plugin running on non linux system!";
        return false;
    } else {
        if (std::string(sys_name.machine).substr(0, 3) == "x86") {
            return collect_x86();
        } else if (std::string(sys_name.machine).substr(0, 3) == "ppc") {
            return collect_ppc();
        } else {
            LOG_ERROR << "Machine type not supported!";
            return false;
        }
    }
}

bool CPU::collect_x86()
{
    bool is_ok = true;
    LOG_INFO << "Parsing " << this->Name() << " info...";
      
    /* Open /proc/cpuinfo for reading. This should be perfectly fine on linux */
    std::ifstream ifs(CPU_INFO_FILE);
    if (ifs.fail()) {
        LOG_ERROR << "Could not open " << CPU_INFO_FILE;
        return false;
    }

    /* parse all content into cpu_info string, used later for regex matching */
    std::string cpu_info( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );
    LOG_VERBOSE << "Content of CPU_INFO_FILE: " << cpu_info;

    /*  == Get CPU Model ==  */
   std::vector<std::string> s_model = DoRegexMatch(cpu_info, "[mM]odel name[ \t]+:[ \t]+(.*)");

    if ( s_model.size()  > 0 ) {
        try {
            std::vector<std::string> model(s_model.size());
            /* trim the string */
            std::transform(s_model.begin(), s_model.end(), model.begin(), [](const std::string& val)
            {
                return trim_copy(val);
            });
            
      
            Item<std::string> i = Item<std::string>("cpuModel", MATCHALL , model);
            m_items.strings.push_back(i);
        } catch (const std::exception &e) 
        { 
            LOG_ERROR << "Error in CPU::Collect: " << e.what(); is_ok = false;
        }
    } else {
        LOG_ERROR << "Cannot parse CPU model.";
        is_ok = false;
    }
    
    
    
    
    /*  == Get CPU Frequency, in MHz ! ==  */
    std::vector<std::string> s_freq = DoRegexMatch(cpu_info, "cpu MHz[ \t]+: (.*)");
    if (s_freq.size()  > 0 ) {
        std::vector<float> freq(s_freq.size());
        try {
            /* Convert from string to float with labda function */
            std::transform(s_freq.begin(), s_freq.end(), freq.begin(), [](const std::string& val)
            {
                return std::stof(val);
            });
            
            Item<float> f = Item<float>("cpuMhz", "MHz", MATCHALL,freq);
            m_items.floats.push_back(f);
        } catch (const std::exception &e) 
        { 
            LOG_ERROR << "Error in CPU::Collect: " << e.what(); is_ok = false;
        }
        
    } else {
        LOG_ERROR << "Cannot parse CPU Freq.";
        is_ok = false;
    }



    /*  == Get cache size, in KB ! ==  */
    std::vector<std::string> s_cpu_active = DoRegexMatch(cpu_info, "processor[ \t]+: ([1-9][0-9]*|0)+");
    if (s_cpu_active.size()  > 0 ) {
        
        try {
            int level_idx = 0;
            bool more_levels = true;
            while (more_levels) {
                std::vector<float> cache;
                std::string level, size, ctype;
                for (int i = 0; i < s_cpu_active.size(); i++ ) {
                    // cache level idx
                    more_levels = collect_cache(s_cpu_active[i], 
                                                std::to_string(level_idx), 
                                                level, size, ctype);
                    if (more_levels == false)
                        break;
                    cache.push_back(std::stof(size));
                }
                if (more_levels == false)
                    break;
                
                if (ctype == "Instruction") {
                    level_idx++;
                    continue;
                }
                    
                Item<float> f = Item<float>("cpuCache"+level, "kB", MATCHALL, cache);
                m_items.floats.push_back(f);
                level_idx++;
            }
        } catch (const std::exception &e)
        { 
            LOG_ERROR << "Error in CPU::Collect: " << e.what(); is_ok = false;
        }
    } else {
        LOG_ERROR << "Cannot parse CPU cache.";
        is_ok = false;
    }
    
    return is_ok;
};


bool CPU::collect_ppc()
{
    bool is_ok = true;
    LOG_INFO << "Parsing " << this->Name() << " info...";
      
    /* Open /proc/cpuinfo for reading. This should be perfectly fine on linux */
    std::ifstream ifs(CPU_INFO_FILE);
    if (ifs.fail()) {
        LOG_ERROR << "Could not open " << CPU_INFO_FILE;
        return false;
    }

    /* parse all content into cpu_info string, used later for regex matching */
    std::string cpu_info( (std::istreambuf_iterator<char>(ifs) ),
                        (std::istreambuf_iterator<char>()    ) );  
    LOG_VERBOSE << "Content of CPU_INFO_FILE: " << cpu_info;

    /*  == Get CPU Model ==  */
    std::vector<std::string> s_model = DoRegexMatch(cpu_info, "cpu[ \t]+:(.*)");
    if ( s_model.size()  > 0 ) {
        try {
            std::vector<std::string> model(s_model.size());
            /* trim the string */
            std::transform(s_model.begin(), s_model.end(), model.begin(), [](const std::string& val)
            {
                return trim_copy(val);
            });
            
            Item<std::string> i = Item<std::string>("cpuModel", MATCHALL , model);
            m_items.strings.push_back(i);
        } catch (const std::exception &e) 
        { 
            LOG_ERROR << "Error in CPU::Collect: " << e.what(); is_ok = false;
        }
    } else {
        LOG_ERROR << "Cannot parse CPU model.";
        is_ok = false;
    }
    
    
    
    
    /*  == Get CPU Frequency, in MHz ! ==  */
    std::vector<std::string> s_freq = DoRegexMatch(cpu_info, "clock[ \t]+:[ \t]([1-9][0-9]+\.[0-9]+)[Mhz| Mhz]");
    if (s_freq.size()  > 0 ) {
        std::vector<float> freq(s_freq.size());
        try {
            /* Convert from string to float with lambda function */
            std::transform(s_freq.begin(), s_freq.end(), freq.begin(), [](const std::string& val)
            {
                return std::stof(val);
            });
            
            Item<float> f = Item<float>("cpuMhz", "MHz", MATCHALL,freq);
            m_items.floats.push_back(f);
        } catch (const std::exception &e) 
        { 
            LOG_ERROR << "Error in CPU::Collect: " << e.what(); is_ok = false;
        }
        
    } else {
        LOG_ERROR << "Cannot parse CPU Freq.";
        is_ok = false;
    }



    /*  == Get cache size, in KB ! ==  */
    std::vector<std::string> s_cpu_active = DoRegexMatch(cpu_info, "processor[ \t]+: ([1-9][0-9]*|0)+");
    if (s_cpu_active.size()  > 0 ) {
        
        try {
            int level_idx = 0;
            bool more_levels = true;
            while (more_levels) {
                std::vector<float> cache;
                std::string level, size, ctype;
                for (int i = 0; i < s_cpu_active.size(); i++ ) {
                    // cache level idx
                    more_levels = collect_cache(s_cpu_active[i], 
                                                std::to_string(level_idx), 
                                                level, size, ctype);
                    if (more_levels == false)
                        break;
                    cache.push_back(std::stof(size));
                }
                if (more_levels == false)
                    break;
                
                if (ctype == "Instruction") {
                    level_idx++;
                    continue;
                }
                    
                Item<float> f = Item<float>("cpuCache"+level, "kB", MATCHALL, cache);
                m_items.floats.push_back(f);
                level_idx++;
            }
        } catch (const std::exception &e)
        { 
            LOG_ERROR << "Error in CPU::Collect: " << e.what(); is_ok = false;
        }
    } else {
        LOG_ERROR << "Cannot parse CPU cache.";
        is_ok = false;
    }
    
    return is_ok;
};

bool CPU::collect_cache(const std::string cpu_num, const std::string idx, std::string& level, std::string& size, std::string& ctype ) {
    std::string fname = std::string(CACHE_INFO_DIR) + cpu_num +  std::string(CACHE_INFO_FILE)+idx+"/";
    
    std::ifstream ifs(fname+"size");
    if (ifs.fail()) {
        return false;
    }
    // check is data type
    std::ifstream ift(fname+"type");    
    std::getline(ift,ctype);
    // check level
    std::ifstream ifl(fname+"level");    
    std::getline(ifl,level);
    // check size
    std::string s;
    std::getline(ifs,s);
    size = s.substr(0, s.size()-1);
    LOG_DEBUG << "Cache info: " << level << size << ctype ;
    return true;
}
