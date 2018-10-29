#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class OMP : public Info
{
public:
    OMP() {};
    ~OMP() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "OMP";}
    std::string Description() {return "Info about the OpenMP library installed on the node";}
    bool FreeText() {return false;}
    bool OnePerNode() {return false;}
        
private:
    Items m_items;
};

class OMPDriver : public InfoDriver
{
public:
    OMPDriver() : InfoDriver("OMPDriver", OMP::version) {}
    Info* create() {return new OMP();}
};
