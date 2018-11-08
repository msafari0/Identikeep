#pragma once
#include <list>
#include <utility> 
#include <vector> 
#include <Info.h>


class FS : public Info
{
public:
    FS() {};
    ~FS() {};
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "FS";}
    std::string Description() {return "Info concerning the FileSystem";}
    bool FreeText() {return false;}
    bool OnePerNode() {return true;}
    
private:
    Items m_items;
};

class FSDriver : public InfoDriver
{
public:
    FSDriver() : InfoDriver("FSDriver", FS::version) {}
    Info* create() {return new FS();}
};
