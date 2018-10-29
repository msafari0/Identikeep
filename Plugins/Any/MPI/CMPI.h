#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class CMPI : public Info
{
public:
    CMPI() {};
    ~CMPI() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "CMPI";}
    std::string Description() {return "Info about the Message Passing Interface implementation installed on the node";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
        
private:
    Items m_items;
};
/* Driver name must match class name */
class CMPIDriver : public InfoDriver
{
public:
    CMPIDriver() : InfoDriver("CMPIDriver", CMPI::version) {}
    Info* create() {return new CMPI();}
};
