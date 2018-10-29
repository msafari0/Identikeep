#pragma once
#include <list>
#include <utility> 
#include <Info.h>


class CPU : public Info
{
public:
    CPU() {};
    ~CPU() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "CPU";}
    std::string Description() {return "SCPU frequency, model, cache etc.";}

    bool FreeText() {return false;}
    bool OnePerNode() {return true;}

private:
    bool collect_x86();
    bool collect_ppc();
    static bool collect_cache(const std::string cpu_num, const std::string idx, std::string& level, std::string& size, std::string& ctype );

    Items m_items;
};

class CPUDriver : public InfoDriver
{
public:
    CPUDriver() : InfoDriver("CPUDriver", CPU::version) {}
    Info* create() {return new CPU();}
};
