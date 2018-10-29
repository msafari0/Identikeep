#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class MKL : public Info
{
public:
    MKL() {};
    ~MKL() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "MKL";}
    std::string Description() {return "Description of the plugin";}
    bool FreeText() {return false;}
    bool OnePerNode() {return false;}
        
private:
    Items m_items;
};

class MKLDriver : public InfoDriver
{
public:
    MKLDriver() : InfoDriver("MKLDriver", MKL::version) {}
    Info* create() {return new MKL();}
};
