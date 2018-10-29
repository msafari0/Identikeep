#pragma once
#include <list>
#include <utility> 
#include <Info.h>


class Libs : public Info
{
public:
    Libs() {};
    ~Libs() {};
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "Libs";}
    std::string Description() {return "Libraries liked to the executable.";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
    
private:
    Items m_items;
};

class LibsDriver : public InfoDriver
{
public:
    LibsDriver() : InfoDriver("LibsDriver", Libs::version) {}
    Info* create() {return new Libs();}
};
