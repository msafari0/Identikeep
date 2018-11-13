#include <sstream>      // std::stringstream
#include <chrono>
#include <string>
#include "plog/Log.h"
#include "PCollect.h"

#include "MPIntercept.h"

#define MASTER 0

using namespace std;



PCollect_options* options;


int MPI_Init(int *argc, char ***argv){
      // Execution starting time is stored
    
    options = new PCollect_options;
    options->LoadEnv();

    int ierr = PMPI_Init( argc, argv );
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

    
    options->Setup_filenames();
    Setup_log(*options);
    

    if(Rank==0) options->Print();
    
    bool found_all=CheckPlugins(options->path, options->required, Rank);
    if(!found_all){
        if(Rank==0) LOG_ERROR << "Unable to find one or more required plugins. Aborting" << std::endl;
        return 0;
    }
      
    
    MPI_Barrier(MPI_COMM_WORLD);

    if(Rank==0) LOG_INFO << "Starting main execution";  

    return ierr;
} 

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{   
   
    options = new PCollect_options;
    options->LoadEnv();

    int ierr = PMPI_Init_thread( argc, argv, required, provided );
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

    
    options->Setup_filenames();
    Setup_log(*options);
    

    if(Rank==0) options->Print();
    
    bool found_all=CheckPlugins(options->path, options->required, Rank);
    if(!found_all){
        if(Rank==0) LOG_ERROR << "Unable to find one or more required plugins. Aborting" << std::endl;
        return 0;
    }
    
        
    MPI_Barrier(MPI_COMM_WORLD);

    if(Rank==0) LOG_INFO << "Starting main execution";
    

    return ierr;
}




int MPI_Finalize(){

    MPI_Barrier(MPI_COMM_WORLD);
   
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);  
    
    
    if(Rank==0) LOG_INFO << "Main execution terminated";

    
    //Analysis function is called
    PCollect(*options);

// 	int result = ((real_MPI_Finalize_t)dlsym(RTLD_NEXT, "MPI_Finalize"))();
   	int result = PMPI_Finalize();

    return result;

};



