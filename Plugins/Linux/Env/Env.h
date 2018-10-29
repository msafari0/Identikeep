#pragma once
#include <list>
#include <utility> 
#include <Info.h>


class Env : public Info
{
public:
    Env() {};
    ~Env() {};
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "Env";}
    std::string Description() {return "Environment variables defined on the system.";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
    
private:
    Items m_items;
};

class EnvDriver : public InfoDriver
{
public:
    EnvDriver() : InfoDriver("EnvDriver", Env::version) {}
    Info* create() {return new Env();}
};
