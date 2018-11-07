#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class IOBench : public Info
{
public:
    IOBench() {};
    ~IOBench() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "IOBench";}
    std::string Description() {return "Short I/O benchmark";}
    bool FreeText() {return false;}
    bool OnePerNode() {return false;}
        
private:
    Items m_items;
};
/* Driver name must match class name */
class IOBenchDriver : public InfoDriver
{
public:
    IOBenchDriver() : InfoDriver("IOBench", IOBench::version) {}
    Info* create() {return new IOBench();}
};
