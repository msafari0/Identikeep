#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class Extensions : public Info
{
public:
    Extensions() {};
    ~Extensions() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "Extensions";}
    std::string Description() {return "Load extensions";}
    bool FreeText() {return false;}
    bool OnePerNode() {return false;}
        
private:
    Items m_items;
};

class ExtensionsDriver : public InfoDriver
{
public:
    ExtensionsDriver() : InfoDriver("ExtensionsDriver", Extensions::version) {}
    Info* create() {return new Extensions();}
};
