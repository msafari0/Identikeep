#include <sstream>      // std::stringstream
#include <chrono>
#include <string>
#include "plog/Log.h"
#include "PCollect.h"

#include "MPIntercept.h"

#define MASTER 0

using namespace std;

extern std::chrono::time_point<std::chrono::system_clock> start_time;
extern std::chrono::time_point<std::chrono::system_clock> end_time;

int MPI_Init(int *argc, char ***argv){
      // Execution starting time is stored
    
    PCollect_options temp_options;
    temp_options.LoadEnv();

    start_time =std::chrono::system_clock::now();

    int ierr = PMPI_Init( argc, argv );
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

    if(Rank==MASTER) temp_options.Print();
    
    bool found_all=CheckPlugins(temp_options.path, temp_options.required, Rank);
    if(!found_all){
        if(Rank==MASTER){
            std::cerr << "IDENTIKIT: ERROR - Unable to find one or more required plugins. Aborting" << std::endl;
            std::cerr << "IDENTIKIT: HINT - Run with option -ls for a list of available plugins." << std::endl;
        }
        PMPI_Finalize();
        exit(0);
    }
    else{
        if(Rank==MASTER) PrintHeader();
    }
        
    MPI_Barrier(MPI_COMM_WORLD);

    

    return ierr;
} 

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{   
   
    PCollect_options temp_options;
    temp_options.LoadEnv();

    start_time =std::chrono::system_clock::now();

    int ierr = PMPI_Init_thread(argc, argv, required, provided);
    
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    
    if(Rank==MASTER) temp_options.Print();
    
    bool found_all=CheckPlugins(temp_options.path, temp_options.required, Rank);
    if(!found_all){
        if(Rank==MASTER){
            std::cerr << "IDENTIKIT: ERROR - Unable to find one or more required plugins. Aborting" << std::endl;
            std::cerr << "IDENTIKIT: HINT - Run with option -ls for a list of available plugins." << std::endl;
        }
        PMPI_Finalize();
        exit(0);
    }
    else{
        if(Rank==MASTER) PrintHeader();
    }
        
    MPI_Barrier(MPI_COMM_WORLD);

    

    return ierr;
}




int MPI_Finalize(){

    MPI_Barrier(MPI_COMM_WORLD);
    
    PCollect_options options;
    options.LoadEnv();
    
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

    if(Rank==MASTER) PrintFooter();
    
    //Analysis function is called
    PCollect(options);

// 	int result = ((real_MPI_Finalize_t)dlsym(RTLD_NEXT, "MPI_Finalize"))();
   	int result = PMPI_Finalize();

    return result;

};



