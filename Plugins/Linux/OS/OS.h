#pragma once
#include <list>
#include <utility> 
#include <Info.h>


class OS : public Info
{
public:
    OS() {};
    ~OS() {};
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "OS";}
    std::string Description() {return "Info concerning the Operative System running on the node";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
    
private:
    Items m_items;
};

class OSDriver : public InfoDriver
{
public:
    OSDriver() : InfoDriver("OSDriver", OS::version) {}
    Info* create() {return new OS();}
};
