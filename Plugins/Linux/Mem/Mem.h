#pragma once
#include <list>
#include <utility> 
#include <Info.h>


class MEM : public Info
{
public:
    MEM() {};
    ~MEM() {};
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "MEM";}
    std::string Description() {return "Info concerning the memory installed on the node";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
    
private:
    Items m_items;
};

class MEMDriver : public InfoDriver
{
public:
    MEMDriver() : InfoDriver("MEMDriver", MEM::version) {}
    Info* create() {return new MEM();}
};
