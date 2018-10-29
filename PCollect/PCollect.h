#pragma once
#include <string>



class PCollect_options{
    public:
        PCollect_options();
        ~PCollect_options(){};
        void LoadEnv();
        void Print();
        std::string path;
        std::string comment;
        std::string tag;
        std::string log_level;
        std::vector<std::string> blacklisted;
        std::vector<std::string> required;
        
        bool identikit;
};

void PCollect(PCollect_options &options);
void ListPlugins(std::string plugin_path);
bool CheckPlugins(std::string plugin_path, std::vector<std::string> required, int rank);
void PrintHeader();
void PrintFooter();


