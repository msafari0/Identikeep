#include <mpi.h>
#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <vector>
#include "plog/Log.h"

#include "PCollect.h"

// Arg parsing
#include "argh.h"



void usage(const char* exec_name) {
    std::cout << exec_name << "[OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "-ls : list available inspectors at the given PLUGIN_PATH to stdout." << std::endl;
    std::cout << "-p=/path or --plugins=/path : set plugins path." << std::endl;
    std::cout << "-c=\"...\" or --comment=\"..\" : add a comment (free text in general) to the output file." << std::endl;
    std::cout << "-t=\"...\" or --tag=\"..\" : add a tag to the output file name." << std::endl;
    std::cout << "[PLUGIN_NAME : require the plugin PLUGIN_NAME. Execution will abort if the plugin PLUGIN_NAME is not found" << std::endl;  
    std::cout << "]PLUGIN_NAME : exclude the plugin PLUGIN_NAME from the analysis, if present among the plugins." << std::endl;  
    std::cout << std::endl;
    std::cout << "--trace : print everything the code does." << std::endl;
    std::cout << "--debug : print debug info." << std::endl;
    std::cout << "--verbose : print debug info." << std::endl;
}


int main(int argc, char *argv[])
{
    
//     int ierr = PMPI_Init ( &argc, &argv );
    
    int prov_thread_level=-1;
    int ierr = PMPI_Init_thread(&argc, &argv , MPI_THREAD_MULTIPLE , &prov_thread_level);
    
    if (ierr != 0) {
        std::cerr << "FATAL: could not initialize MPI." << std::endl;
        return -1;
    }
    
    int Rank=0;
    int Size=1;

    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    MPI_Comm_size(MPI_COMM_WORLD, &Size);

    PCollect_options options;
    auto cmdl = argh::parser(argc, argv);

    //Print help 
    if(cmdl[{"-h","--help"}]){
        if(Rank==0) usage(argv[0]);
        return 0;
    }
    
    //Check if path is present and save it
    
    
    options.path=cmdl({ "-p", "--plugins"}, "").str();
    
    if(options.path==""){
       options.path=".";        
    }
    
    //List plugins and exit
    if(cmdl[{"-ls","--list"}]){
        if(Rank==0) ListPlugins(options.path);
        return 0;
    }
    
    
    //save comments
    options.comment =cmdl({ "-c", "--comment"}, "").str() ;
    

    //save tag
    options.tag=cmdl({ "-t", "--tag"}, "").str();

    
    
    if(cmdl[{"--trace"}]) options.log_level="trace";
    else if(cmdl[{"--debug"}]) options.log_level="debug";
    else if(cmdl[{"--verbose"}]) options.log_level="verbose";
    
    
    
    
    std::string prefix("]");
    for(int i=0; i<argc; i++){
        std::string arg(argv[i]);
        if (!arg.compare(0, prefix.size(), prefix)){
            options.blacklisted.push_back(arg.substr(prefix.size()));
        }      
    }
    

    prefix="[";
    for(int i=0; i<argc; i++){
        std::string arg(argv[i]);
        if (!arg.compare(0, prefix.size(), prefix)){
            options.required.push_back(arg.substr(prefix.size()));
        }      
    }

    options.Setup_filenames();
    Setup_log(options);
    
    

    if(Rank==0) options.Print();
    
    bool found_all=CheckPlugins(options.path, options.required, Rank);
    if(!found_all){
        if(Rank==0) LOG_ERROR << "Unable to find one or more required plugins. Aborting" << std::endl;
        return 0;
    }
    
    //Analysis function is called
    PCollect(options);
    
    
    
    


    PMPI_Finalize ();

    return 0;
}
