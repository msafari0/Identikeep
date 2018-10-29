#pragma once
#include <list>
#include <utility> 
#include <Info.h>


class Exec : public Info
{
public:
    Exec() {};
    ~Exec() {};
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "Exec";}
    std::string Description() {return "Information about the executable, like its path, creation date and size.";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
    
private:
    Items m_items;
};

class ExecDriver : public InfoDriver
{
public:
    ExecDriver() : InfoDriver("ExecDriver", Exec::version) {}
    Info* create() {return new Exec();}
};
