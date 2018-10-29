#include <vector>
#include <string>
#include "Info.h"
#include <pugg/Kernel.h>


extern bool MPI_FLAG;

class PluginSystem
{
public:
    PluginSystem();
    ~PluginSystem(){};
    
    bool LoadInfoInstances(const std::string plugin_path, std::vector<Info*> &);
    
private:    
    pugg::Kernel m_kernel;
    
};
