#include <mpi.h>
#include "MPIBench.h"
#include <cmath>
#include <iostream>
// Logging
#include "plog/Log.h"

#define MATCHALL match::list | match::node | match::world

#define NTRIES 64
#define MASTER 0
#define SIZE 1024*1024


using namespace std;

bool    CommByNode(MPI_Comm &NodeComm,
                MPI_Comm &MasterComm,
                int &NodeRank, int &MasterRank,
                int &NodeSize, int &MasterSize,
                string &NodeNameStr);


bool MPIBench::Collect(int argc, char *argv[])
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
    
    
    int Rank_global=0;
    int Size_global=1;

    MPI_Comm_rank(MPI_COMM_WORLD, &Rank_global);
    MPI_Comm_size(MPI_COMM_WORLD, &Size_global);
    
    
    MPI_Comm NodeComm, MasterComm;
    string nodename;
    int node_rank, Rank, node_size, Size;
    
    CommByNode(NodeComm, MasterComm, node_rank, Rank, node_size, Size, nodename);
    
    
    
    if(Size>1 and node_rank==0){
        
    char Names[Size][MPI_MAX_PROCESSOR_NAME];    
    strcpy(Names[Rank], nodename.c_str());
    
    for(int i_master=0; i_master<Size; i_master++){
        MPI_Bcast( &Names[i_master][0],MPI_MAX_PROCESSOR_NAME , MPI_CHAR, i_master, MasterComm );
    }
        
        
    char test_data;
    char* test_array=new char[SIZE];
    
    int array_size=SIZE*sizeof(char);

    double* ping_time_s=new double[Size];
    double* ping_time_ss=new double[Size];

    double* bandwidth_s=new double[Size];
    double* bandwidth_ss=new double[Size];    
    
    double bcast_s=0;
    double bcast_ss=0;
    
    for(int i=0; i<Size; i++){
        ping_time_s[i]=0;
        ping_time_ss[i]=0;
        bandwidth_s[i]=0;
        bandwidth_ss[i]=0;
        
    }        

    double start,end, diff;
    
    for(int itries=0; itries<NTRIES; itries++){
            if(Rank==MASTER){
            for(int i_recv=0; i_recv<Size; i_recv++){
                if(i_recv!=Rank){
                    //ping pong 
                    start = MPI_Wtime();
                    MPI_Send(&test_data, 1, MPI_CHAR, i_recv, 0, MasterComm);
                    MPI_Recv(&test_data, 1, MPI_CHAR, i_recv, 0, MasterComm, MPI_STATUS_IGNORE);
                    end = MPI_Wtime();
                    diff=(end - start)*1000.;
                    ping_time_s[i_recv]+=diff;
                    ping_time_ss[i_recv]+=diff*diff;
                    
                    //ping pong bandwidth
                    start = MPI_Wtime();
                    MPI_Send(test_array, SIZE, MPI_CHAR, i_recv, 0, MasterComm);
                    MPI_Recv(test_array, SIZE, MPI_CHAR, i_recv, 0, MasterComm, MPI_STATUS_IGNORE);
                    end = MPI_Wtime();
                    diff=double(array_size)/(end - start)*2./1000000.;
                    bandwidth_s[i_recv]+=diff;
                    bandwidth_ss[i_recv]+=diff*diff;
                                        
                }          
            }
            }
            else{
                MPI_Recv(&test_data, 1, MPI_CHAR, MASTER, 0, MasterComm, MPI_STATUS_IGNORE);
                MPI_Send(&test_data, 1, MPI_CHAR, MASTER, 0, MasterComm);

                MPI_Recv(test_array, SIZE, MPI_CHAR, MASTER, 0, MasterComm, MPI_STATUS_IGNORE);
                MPI_Send(test_array, SIZE, MPI_CHAR, MASTER, 0, MasterComm);
                }
                
                //bcast
            
            
            
            //bcast
            MPI_Barrier(MasterComm);
            start = MPI_Wtime();
            MPI_Bcast( test_array, SIZE, MPI_CHAR, MASTER, MasterComm );
            MPI_Barrier(MasterComm);
            end = MPI_Wtime();
            diff=double(array_size)/((end - start)*1000000.);
            bcast_s+=diff;
            bcast_ss+=diff*diff;            

    }
    
    
    MPI_Bcast(ping_time_s, Size, MPI_DOUBLE, 0, MasterComm);
    MPI_Bcast(ping_time_ss, Size, MPI_DOUBLE, 0, MasterComm);
    MPI_Bcast(bandwidth_s, Size, MPI_DOUBLE, 0, MasterComm);
    MPI_Bcast(bandwidth_ss, Size, MPI_DOUBLE, 0, MasterComm);

    
    
    if(Rank==MASTER){
       
        ////bcast
        
        double bcast=0;
        double bcast_dev=0;
        bcast=bcast_s/double(NTRIES);
        bcast_dev=sqrt(bcast_ss/double(NTRIES) - bcast*bcast);
        int bcast_perc = int( bcast_dev / bcast * 100. + 0.5);
      
        std::string title="mpibenchBandwidthBcast - Pack size: " +std::to_string(array_size/1000000.) + "MB";
        Item<float> i = Item<float>(title, "MB/s", MATCHALL, bcast);
        m_items.floats.push_back(i);
    }
    else{

        double ping_time=0;
        double ping_time_dev=0;
        ping_time=ping_time_s[Rank]/double(NTRIES);
        ping_time_dev=sqrt(ping_time_ss[Rank]/double(NTRIES) - ping_time*ping_time);
        int ping_time_perc = int( ping_time_dev / ping_time * 100. + 0.5);


        double bandwidth=0;
        double bandwidth_dev=0;
        bandwidth=bandwidth_s[Rank]/double(NTRIES);
        bandwidth_dev=sqrt(bandwidth_ss[Rank]/double(NTRIES) - bandwidth*bandwidth);
        int bandwidth_perc = int( bandwidth_dev / bandwidth * 100. + 0.5);
            
        
        std::string title="mpibenchPingPong";
        Item<float> i = Item<float>(title, "ms", MATCHALL, ping_time );
        m_items.floats.push_back(i);

        title="mpibenchPingPongBandwidth - Pack size: " +std::to_string(array_size/1000000.) + "MB";
        i = Item<float>(title, "MB/s", MATCHALL, bandwidth);
        m_items.floats.push_back(i); 
        
    }
    
    
    delete [] ping_time_s;
    delete [] ping_time_ss;
    delete [] bandwidth_s;
    delete [] bandwidth_ss;
    
    }

    return is_ok;
};







/* This (not so nice) code was taken from:
 * 
 * https://stackoverflow.com/questions/14411763/mapping-mpi-processes-to-particular-nodes
 * 
 * Make a comunicator for each node and another for just
 * the masters of the nodes. Upon completion, everyone is
 * in a new node communicator, knows its size and their rank,
 * and the rank of their master in the master communicator,
 * which can be useful to use for indexing.
 */
bool    CommByNode(MPI_Comm &NodeComm,
                MPI_Comm &MasterComm,
                int &NodeRank, int &MasterRank,
                int &NodeSize, int &MasterSize,
                string &NodeNameStr)
{
    bool IsOk = true;

    int Rank;
    int Size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    MPI_Comm_size(MPI_COMM_WORLD, &Size);

    if(Rank==MASTER) LOG_INFO << "Preparing communicators...";
    else LOG_VERBOSE << "Preparing communicators...";
    /*
     * ======================================================================
     * What follows is my best attempt at creating a communicator
     * for each node in a job such that only the cores on that
     * node are in the node's communicator, and each core groups
     * itself and the node communicator is made using the Split() function.
     * The end of this (lengthly) process is indicated by another comment.
     * ======================================================================
     */
    char *NodeName, *NodeNameList;
    NodeName = new char [MPI_MAX_PROCESSOR_NAME]; // REPLACE THIS!!
    int NodeNameLen,
        *NodeNameCountVect,
        *NodeNameOffsetVect,
        NodeNameTotalLen = 0;
    //  Get the name and name character count of each core's node
    MPI_Get_processor_name(NodeName, &NodeNameLen);
    
    //  Prepare a vector for character counts of node names
    if (Rank == MASTER)
        NodeNameCountVect = new int [Size];

    //  Gather node name lengths to master to prepare c-array
    MPI_Gather(&NodeNameLen, 1, MPI_INT, NodeNameCountVect, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    if (Rank == MASTER){
        //  Need character count information for navigating node name c-array
        NodeNameOffsetVect = new int [Size];
        NodeNameOffsetVect[0] = 0;
        NodeNameTotalLen = NodeNameCountVect[0];

        //  build offset vector and total char count for all node names
        for (int i = 1 ; i < Size ; ++i){
            NodeNameOffsetVect[i] = NodeNameCountVect[i-1] + NodeNameOffsetVect[i-1];
            NodeNameTotalLen += NodeNameCountVect[i];
        }
        //  char-array for all node names
        NodeNameList = new char [NodeNameTotalLen];
    }

    //  Gatherv node names to char-array in master
    MPI_Gatherv(NodeName, NodeNameLen, MPI_CHAR, NodeNameList, NodeNameCountVect, NodeNameOffsetVect, MPI_CHAR, MASTER, MPI_COMM_WORLD);

    string *FullStrList, *NodeStrList;
    //  Each core keeps its node's name in a str for later comparison
    stringstream ss;
    ss << NodeName;
    ss >> NodeNameStr;

    delete NodeName;    //  node name in str, so delete c-array

    int *NodeListLenVect, NumUniqueNodes = 0, NodeListCharLen = 0;
    string NodeListStr;

    if (Rank == MASTER){
        /*
         * Need to prepare a list of all unique node names, so first
         * need all node names (incl duplicates) as strings, then
         * can make a list of all unique node names.
         */
        FullStrList = new string [Size];    //  full list of node names, each will be checked
        NodeStrList = new string [Size];    //  list of unique node names, used for checking above list
        //  i loops over node names, j loops over characters for each node name.
        for (int i = 0 ; i < Size ; ++i){
            stringstream ss;
            for (int j = 0 ; j < NodeNameCountVect[i] ; ++j)
                ss << NodeNameList[NodeNameOffsetVect[i] + j];  //  each char into the stringstream
            ss >> FullStrList[i];   //  stringstream into string for each node name
            ss.str(""); //  This and below clear the contents of the stringstream,
            ss.clear(); //  since the >> operator doesn't clear as it extracts
        }
        delete NodeNameList;    //  master is done with full c-array
        bool IsUnique;  //  flag for breaking from for loop
        stringstream ss;    //  used for a full c-array of unique node names
        for (int i = 0 ; i < Size ; ++i){   //  Loop over EVERY name
            IsUnique = true;
            for (int j = 0 ; j < NumUniqueNodes ; ++j)
                if (FullStrList[i].compare(NodeStrList[j]) == 0){   //  check against list of uniques
                    IsUnique = false;
                    break;
                }
            if (IsUnique){
                NodeStrList[NumUniqueNodes] = FullStrList[i];   //  add unique names so others can be checked against them
                ss << NodeStrList[NumUniqueNodes].c_str();  //  build up a string of all unique names back-to-back
                ++NumUniqueNodes;   //  keep a tally of number of unique nodes
            }
        }
        ss >> NodeListStr;  //  make a string of all unique node names
        NodeListCharLen = NodeListStr.size();   //  char length of all unique node names
        NodeListLenVect = new int [NumUniqueNodes]; //  list of unique node name lengths
        /*
         * Because Bcast simply duplicates the buffer of the Bcaster to all cores,
         * the buffer needs to be a char* so that the other cores can have a similar
         * buffer prepared to receive. This wouldn't work if we passed string.c_str()
         * as the buffer, becuase the receiving cores don't have string.c_str() to
         * receive into, and even if they did, c_srt() is a method and can't be used
         * that way.
         */
        NodeNameList = new char [NodeListCharLen];  //  even though c_str is used, allocate necessary memory
        NodeNameList = const_cast<char*>(NodeListStr.c_str());  //  c_str() returns const char*, so need to recast
        for (int i = 0 ; i < NumUniqueNodes ; ++i)  //  fill list of unique node name char lengths
            NodeListLenVect[i] = NodeStrList[i].size();
        /*for (int i = 0 ; i < NumUnique ; ++i)
            cout << UniqueNodeStrList[i] << endl;
        MPI::COMM_WORLD.Abort(1);*/
        //delete NodeStrList;   //  Arrays of string don't need to be deallocated,
        //delete FullStrList;   //  I'm guessing becuase of something weird in the string class.
        delete NodeNameCountVect;
        delete NodeNameOffsetVect;
    }
    

    /*
     * Now we send the list of node names back to all cores
     * so they can group themselves appropriately.
     */

    //  Bcast the number of nodes in use
    MPI_Bcast(&NumUniqueNodes, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    //  Bcast the full length of all node names
    MPI_Bcast(&NodeListCharLen, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    //  prepare buffers for node name Bcast's
    if (Rank > MASTER){
        NodeListLenVect = new int [NumUniqueNodes];
        NodeNameList = new char [NodeListCharLen];
    }

    //  Lengths of node names for navigating c-string
    MPI_Bcast(NodeListLenVect, NumUniqueNodes, MPI_INT, MASTER, MPI_COMM_WORLD);
    //  The actual full list of unique node names
    MPI_Bcast(NodeNameList, NodeListCharLen, MPI_CHAR, MASTER, MPI_COMM_WORLD);

    /*
     * Similar to what master did before, each core (incl master)
     * needs to build an actual list of node names as strings so they
     * can compare the c++ way.
     */
    int Offset = 0;
    NodeStrList = new string[NumUniqueNodes];
    for (int i = 0 ; i < NumUniqueNodes ; ++i){
        stringstream ss;
        for (int j = 0 ; j < NodeListLenVect[i] ; ++j)
            ss << NodeNameList[Offset + j];
        ss >> NodeStrList[i];
        ss.str("");
        ss.clear();
        Offset += NodeListLenVect[i];
        //cout << FullStrList[i] << endl;
    }
    //  Now since everyone has the same list, just check your node and find your group.
    int CommGroup = -1;
    for (int i = 0 ; i < NumUniqueNodes ; ++i)
        if (NodeNameStr.compare(NodeStrList[i]) == 0){
            CommGroup = i;
            break;
        }
    if (Rank > MASTER){
        delete NodeListLenVect;
        delete NodeNameList;
    }
    //  In case process fails, error prints and job aborts.
    if (CommGroup < 0){
        LOG_ERROR << "Rank "  << Rank << " didn't identify comm group correctly.";
        IsOk = false;
    }

    /*
     * ======================================================================
     * The above method uses c++ strings wherever possible so that things
     * like node name comparisons can be done the c++ way. I'm sure there's
     * a better way to do this because that was way too many lines of code...
     * ======================================================================
     */

    //  Create node communicators
    MPI_Comm_split(MPI_COMM_WORLD, CommGroup, 0, &NodeComm);
    MPI_Comm_rank(NodeComm, &NodeRank);
    MPI_Comm_size(NodeComm, &NodeSize);

    //  Group for master communicator
    int MasterGroup;
    if (NodeRank == MASTER)
        MasterGroup = 0;
    else
        MasterGroup = MPI_UNDEFINED;


    //  Create master communicator
    MPI_Comm_split(MPI_COMM_WORLD, MasterGroup, 0, &MasterComm);
    MasterRank = -1;
    MasterSize = -1;

    if (MasterComm != MPI_COMM_NULL){
        MPI_Comm_rank(MasterComm, &MasterRank);
        MPI_Comm_size(MasterComm, &MasterSize);
    }

    MPI_Bcast(&MasterSize, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&MasterRank, 1, MPI_INT, MASTER, NodeComm);
    return IsOk;
}












