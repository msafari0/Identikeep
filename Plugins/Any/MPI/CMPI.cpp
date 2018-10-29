#include <mpi.h>
#include "CMPI.h"

// Logging
#include "plog/Log.h"

#define MATCHALL match::list | match::node | match::world

bool CMPI::Collect(int argc, char *argv[])
{
    
    bool is_ok = true;
    int version, subversion;

    LOG_INFO << "Parsing " << this->Name() << " info...";
    
    int mpiflag;
    MPI_Initialized(&mpiflag);
    if (!mpiflag) {
        LOG_ERROR << "MPI not initialized!";
        return false;
    }

    MPI_Get_version(&version, &subversion);

    Item<int> i = Item<int>("mpiVersion", MATCHALL, version);
    m_items.integers.push_back(i);
    i = Item<int>("mpiSubversion", MATCHALL, subversion);
    m_items.integers.push_back(i);

    return is_ok;
};
