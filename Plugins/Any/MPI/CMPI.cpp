#include <mpi.h>
#include "CMPI.h"

// Logging
#include "plog/Log.h"

#define MATCHALL match::list | match::node | match::world

bool CMPI::Collect(int argc, char *argv[])
{
    
    bool is_ok = true;
    int version, subversion;

    LOG_VERBOSE << "Parsing " << this->Name() << " info...";
    
    int mpiflag;
    MPI_Initialized(&mpiflag);
    if (!mpiflag) {
        LOG_ERROR << "MPI not initialized!";
        return false;
    }

    MPI_Get_version(&version, &subversion);

    int provided_thread_level=-1;
    MPI_Query_thread(&provided_thread_level);
    std::string thread_descr("");
    if(provided_thread_level==MPI_THREAD_SINGLE) thread_descr="(single)";
    if(provided_thread_level==MPI_THREAD_FUNNELED) thread_descr="(funneled)";
    if(provided_thread_level==MPI_THREAD_SERIALIZED) thread_descr="(serialized)";
    if(provided_thread_level==MPI_THREAD_MULTIPLE) thread_descr="(multiple)";
    
    
    Item<int> i = Item<int>("mpiVersion", MATCHALL, version);
    m_items.integers.push_back(i);
    i = Item<int>("mpiSubversion", MATCHALL, subversion);
    m_items.integers.push_back(i);
    i = Item<int>("mpiThreadLevel", thread_descr,  MATCHALL, provided_thread_level);
    m_items.integers.push_back(i);
    return is_ok;
};
