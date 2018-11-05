#pragma once
#include <string>
#include <vector> 
#include <utility> 
#include <Info.h>


class CUDA : public Info
{
public:
    CUDA() {};
    ~CUDA() {}
    bool Collect(int argc, char *argv[]);
    Items GetItems(){return m_items;};

    
    std::string Name() {return "CUDA";}
    std::string Description() {return "Info about accelerators via CUDA library";}
    bool FreeText() {return false;}
    bool OnePerNode() {return false;}
        
private:
    Items m_items;
};

class CUDADriver : public InfoDriver
{
public:
    CUDADriver() : InfoDriver("CUDADriver", CUDA::version) {}
    Info* create() {return new CUDA();}
};
