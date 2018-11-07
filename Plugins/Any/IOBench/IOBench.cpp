#include <mpi.h>
#include <stdio.h>
#include "IOBench.h"
#include <cmath>
#include <iostream>
// Logging
#include "plog/Log.h"

#define MATCHALL match::list | match::node | match::world

#define FILENAME "IOBench.temp_"
#define MINSIZE 1024*1024
#define TRIES 8

using namespace std;

bool IOBench::Collect(int argc, char *argv[])
{
    
    double tt;
    bool is_ok = true;

    LOG_VERBOSE << "Parsing " << this->Name() << " info...";

    int Rank_global=0;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank_global);
    
    std::vector<float> write_speed;
    std::vector<float> read_speed;
    std::vector<float> file_size_mb;
    
    std::vector<int> file_size;
    file_size.push_back(MINSIZE);
    file_size.push_back(MINSIZE*8);
    file_size.push_back(MINSIZE*64);
    file_size.push_back(MINSIZE*512);
    
    std::string filename=FILENAME+std::to_string(Rank_global);
    
    
for(int isize=0; isize<file_size.size(); isize++){
    unsigned int FSIZE = file_size[isize];
    
    int   write_speed_dev_perc;
    float write_sum=0;
    float write_ssum=0;
    
    int   read_speed_dev_perc;
    float read_sum=0;
    float read_ssum=0;
    
    FILE * pFile;
    char *buffer = new char[FSIZE];
    
    for(int i_tries=0; i_tries<TRIES; i_tries++){
        pFile = fopen (filename.c_str(), "wb");
        tt=MPI_Wtime();
        fwrite (buffer , sizeof(char), FSIZE, pFile);
        tt=MPI_Wtime()-tt;
        fclose (pFile);
        float temp_time= float(tt);
        float temp_speed= float(FSIZE*sizeof(char))/(temp_time*1000000.);
        write_sum+=temp_speed;
        write_ssum+=temp_speed*temp_speed;
        
        pFile = fopen (filename.c_str(), "rb");
        tt=MPI_Wtime();       
        fread(buffer , sizeof(char), FSIZE, pFile);
        tt=MPI_Wtime()-tt;
        fclose (pFile);
        temp_time= float(tt);
        temp_speed= float(FSIZE*sizeof(char))/(temp_time*1000000.);
        read_sum+=temp_speed;
        read_ssum+=temp_speed*temp_speed;
        
        remove(filename.c_str());
        
    }
    
    delete [] buffer;
    
//     write_speed= write_sum/float(TRIES);
//     write_speed_dev=sqrt(write_ssum/float(TRIES)-write_speed*write_speed);
//     write_speed_dev_perc=int(write_speed_dev/write_speed*100.+0.5);
//    
//     
//     std::string title="iobenchWriteSpeed";
//     std::string unit="MB/s (+/- "+std::to_string(write_speed_dev_perc)+"%) - Pack size: " +std::to_string(sizeof(char)*FSIZE/1000000.) + "MB";
//     Item<float> iwrite = Item<float>(title, unit, MATCHALL, write_speed);
//     m_items.floats.push_back(iwrite);
//     
//     
//     
//     read_speed= read_sum/float(TRIES);
//     read_speed_dev=sqrt(read_ssum/float(TRIES)-read_speed*read_speed);
//     read_speed_dev_perc=int(read_speed_dev/read_speed*100.+0.5);
//    
//     title="iobenchReadSpeed";
//     unit="MB/s (+/- "+std::to_string(read_speed_dev_perc)+"%) - Pack size: " +std::to_string(sizeof(char)*FSIZE/1000000.) + "MB";
//     Item<float> iread = Item<float>(title, unit, MATCHALL, read_speed);
//     m_items.floats.push_back(iread);
    
    file_size_mb.push_back(float(FSIZE)/1000000.);
    write_speed.push_back(write_sum/float(TRIES));
    read_speed.push_back(read_sum/float(TRIES));

}
    

    std::string title="iobenchFileSize";
    std::string unit="MB";
    Item<float> ifilesize = Item<float>(title, unit, MATCHALL, file_size_mb);
    m_items.floats.push_back(ifilesize);
       
    
    title="iobenchWriteSpeed";
    unit="MB/s";
    Item<float> iwrite = Item<float>(title, unit, MATCHALL, write_speed);
    m_items.floats.push_back(iwrite);
    
    title="iobenchReadSpeed";
    unit="MB/s";
    Item<float> iread = Item<float>(title, unit, MATCHALL, read_speed);
    m_items.floats.push_back(iread);
    

    return is_ok;
};









