#include <mpi.h>
#include <string>
#define MASTER 0


bool    CommByNode(MPI_Comm &NodeComm,
                MPI_Comm &MasterComm,
                int &NodeRank, int &MasterRank,
                int &NodeSize, int &MasterSize,
                std::string &NodeNameStr);
