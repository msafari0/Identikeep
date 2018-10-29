#include "PluginLoader.h"
#include "Files.h"
// Logging
#include "plog/Log.h"

#include <iostream>
using namespace std;



PluginSystem::PluginSystem() {

    m_kernel.add_server(Info::server_name(), Info::version);
}

/* 
 * Returns a vector of Info* objects. 
 * 
 * 
 */
bool PluginSystem::LoadInfoInstances(const string plugin_path, vector<Info*> & inspectors) {

    
    // Get list of *.so or *.dll files.
    vector<string> pathList = getPathList(plugin_path);
    
    
    
    
    
    for (vector<string>::iterator it = pathList.begin(); it != pathList.end(); ++it){
        bool load_flag=true;
        if(it->find("libMPIntercept.")!=string::npos) load_flag=false;                 // Avoid loading the libPCollect library
       
        
        if(load_flag){
        std::string err_desc;
//         std::cerr << *it << std::endl;

        bool worked = m_kernel.load_plugin(*it, err_desc);
        LOG_DEBUG << "Loading "  <<  *it << "..." << (( worked == true) ? "OK!" : "FAILED!");
//         exit(1);

        if (!worked) {
            LOG_WARNING << " Plugin " << *it << " reports: " << err_desc;
        }
    }
    }

    vector<InfoDriver*> drivers = m_kernel.get_all_drivers<InfoDriver>(Info::server_name());
    
    for (vector<InfoDriver*>::iterator iter = drivers.begin(); iter != drivers.end(); ++iter) {
        InfoDriver& driver = *(*iter);
        LOG_DEBUG << "InfoDriver loaded. Server name: " << driver.server_name() << "; Name: "  <<  driver.name() << "; Version: " << driver.version();
        Info* inspector = driver.create();
        if (inspector) {
            LOG_DEBUG << "Inspector name: " << inspector->Name();
            inspectors.push_back(inspector);
        }
        else
            LOG_ERROR << "Could not create inspector for " <<  driver.name();
    }
    return true;
}

