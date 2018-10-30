#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class MPIBench : public Info
{
public:
    MPIBench() {};
    ~MPIBench() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "MPIBench";}
    std::string Description() {return "Short benchmark of Message Passing Interface";}
    bool FreeText() {return false;}
    bool OnePerNode() {return false;}
        
private:
    Items m_items;
};
/* Driver name must match class name */
class MPIBenchDriver : public InfoDriver
{
public:
    MPIBenchDriver() : InfoDriver("MPIBench", MPIBench::version) {}
    Info* create() {return new MPIBench();}
};
