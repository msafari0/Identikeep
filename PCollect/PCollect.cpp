#include <cstdio>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>
#include <dlfcn.h>
#include <stdlib.h>     /* getenv */
#include <chrono>
#include <iomanip>


// Plugin System

#include "PCollect.h"



#include "PluginLoader.h"

// General info class and MPI functions.
#include "Info.h"
// #include "MPIUtils.h"
// Output
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
// Logging
#include "plog/Log.h"
#include "plog/Appenders/ColorConsoleAppender.h"

// Other minor stuff
#include "Utils.h"

#define MASTER 0
#define PRINTMATCH 0


using namespace std;

// for convenience
using namespace rapidjson;





bool  CommByNode(MPI_Comm &NodeComm,
                MPI_Comm &MasterComm,
                int &NodeRank, int &MasterRank,
                int &NodeSize, int &MasterSize,
                std::string &NodeNameStr);


PCollect_options::PCollect_options():path("."), comment(""), tag(""), log_level(""), output_filename(""), log_filename("") { identikit=true;};

void PCollect_options::Setup_filenames(){
    
        // Setup json filename
    
    output_filename="sysinfo";
    if(tag!="") output_filename+="_"+tag;
    output_filename+=".json";
    
    
    // Setup log filename
    int Rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    
    if(Rank==MASTER) log_filename = output_filename+".log";
    else log_filename = output_filename+".log-" + to_string(Rank);
    
}



void Setup_log(PCollect_options &options){
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(options.log_filename.c_str(), 80000, 1); // Create the 1st appender.
    plog::init(plog::warning, &fileAppender); // Initialize the logger with file appender.

    /* set log level "trace", "debug", "info",  "warning", "error", "critical", "off"  */
    if (options.log_level=="trace") plog::get()->setMaxSeverity(plog::verbose);
    if (options.log_level=="debug") plog::get()->setMaxSeverity(plog::debug);
    if (options.log_level=="verbose") plog::get()->setMaxSeverity(plog::info); 
    if (options.log_level=="") plog::get()->setMaxSeverity(plog::info);
    
    
    
    
}


void PCollect_options::LoadEnv(){
    
    
    std::string ld_preload;
    char* env_ld_preload=getenv("LD_PRELOAD");
    ld_preload=(env_ld_preload==NULL ? "" : env_ld_preload);   
    size_t found=ld_preload.find_last_of("/\\");
    path=ld_preload.substr(0, found);
    
    char* env_path=getenv("IDENTIKEEP_PATH");
    path=(env_path==NULL ? path : env_path);
    
    char* env_comment=getenv("IDENTIKEEP_COMMENT");
    comment=(env_comment==NULL ? "" : env_comment);

    char* env_tag=getenv("IDENTIKEEP_TAG");
    tag=(env_tag==NULL ? "" : env_tag);
    
    char* env_log_level=getenv("IDENTIKEEP_LOG");
    log_level=(env_log_level==NULL ? "" : env_log_level);    
    
    std::string tempstring;

    char* env_blacklist=getenv("IDENTIKEEP_BLACKLIST");
    std::stringstream blacklist_stream(env_blacklist ==NULL ? "" : env_blacklist);
    while(blacklist_stream>>tempstring){blacklisted.push_back(tempstring);}

    char* env_require=getenv("IDENTIKEEP_REQUIRE");
    std::stringstream require_stream(env_require ==NULL ? "" : env_require);
    while(require_stream>>tempstring){required.push_back(tempstring);}
    
    
}


void PCollect_options::Print(){    
    LOG_INFO << "OPTIONS" ;
    LOG_INFO << "--> Plugins path: " << path ;
    if(comment!="") LOG_INFO << "--> Comment: " << comment ;
    if(tag!="")     LOG_INFO << "--> File tag: " << tag ;
    if(log_level!="") LOG_INFO << "--> Log level: " << log_level ;    
    LOG_INFO << "--> Output filename: " << output_filename ;    

    
    if(required.size()>0){
        LOG_INFO << "--> Required plugins: " ;    
        int index=1;
        for(std::vector<std::string>::iterator iter = required.begin(); iter != required.end(); ++iter){
            LOG_INFO << "" << index <<") " << *iter ;
            index++;        
        }
    }
    
    if(blacklisted.size()>0){
        LOG_INFO << "--> Blacklisted plugins: ";    

        int index=1;
        for(std::vector<std::string>::iterator iter = blacklisted.begin(); iter != blacklisted.end(); ++iter){
            LOG_INFO <<  "" << index <<") " << *iter ;
            index++;        
        }
    }
}





////////////////////////////////////////////////////
void PCollect(PCollect_options &options)
{
    
    
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    std::chrono::duration<double> diff_time; 

    int Rank=0;
    int Size=1;

    MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    MPI_Comm_size(MPI_COMM_WORLD, &Size);
    
    if(Rank==MASTER) start_time =std::chrono::system_clock::now();

   

   
    if(Rank==MASTER) LOG_INFO << "Collection started." ;
    
    // variables to store options
    
    // If PCollect is called from the identikit exe, plugins concerning the libraries loaded at runtime (Libs) and the info about the executable (Exec) are disabled. 
//     if(options.identikit){
//         options.blacklisted.push_back("Libs");
//         options.blacklisted.push_back("Exec");       
//     }
    
    
    // The execution time of the executable is saved


     // Variables for MPI
    MPI_Comm node_comm, master_comm;
    int node_rank=0, master_rank=0, node_size=1, master_size=1;
    string NodeNameStr;
   
    
    
///////////////////////
    
    if(Rank==MASTER)  LOG_INFO << "Plugins path set to: "<< options.path;
    
    
    if (DirExists(options.path.c_str())) {
        if(Rank==MASTER)  LOG_VERBOSE << "Plugins path is valid.";
    } else {
        if(Rank==MASTER)  LOG_ERROR << "Plugins path is not valid! No plugins will be found!";
    }
    

    

    /* Split comunicator to have division between nodes. */
    CommByNode(node_comm, master_comm, node_rank, master_rank, node_size, master_size, NodeNameStr);
    if(Rank==MASTER) LOG_INFO << "Starting parallel collection!";
    LOG_DEBUG << "Parallel info " << node_rank << " " << master_rank << " " << node_size << " " << master_size << " " << NodeNameStr;
    
    vector<Info*> inspectors;
    /* What follows is a horrible hack to link statically
     * all plugins in the application.
     * Should be removed soon.
     */

    auto plugin_system = PluginSystem();
    
    plugin_system.LoadInfoInstances(options.path, inspectors);


    // Start inspectors and populate json file.
    // define the document as an object rather than an array
    Document procDoc;
    procDoc.SetObject();
    

    Document::AllocatorType& allocator = procDoc.GetAllocator();
    Value krank("GlobalRank", allocator);
    Value vrank(Rank);
    procDoc.AddMember(krank, vrank, allocator);   
    
    
    
    for(vector<Info*>::iterator iter = inspectors.begin(); iter != inspectors.end(); ++iter) {
        
        Info& inspector = *(*iter);
        

        
        bool check_blacklisted=false;
        for(vector<string>::iterator b_iter = options.blacklisted.begin(); b_iter != options.blacklisted.end(); ++b_iter){
            if((*iter)->Name()==*b_iter){
                LOG_VERBOSE << "Inspector " << (*iter)->Name() << " excluded (blacklisted)";
                check_blacklisted=true;
            }
        }
        if(check_blacklisted) continue;    
        

        if(Rank==MASTER)  LOG_INFO << "Running " << inspector.Name() << " plugin...";

        
        LOG_VERBOSE << "Inspector " 
                << inspector.Name() 
                <<" will run once per node: " 
                << ((inspector.OnePerNode() == true) ? "yes" : "no" )
                << endl;
            
        if ( inspector.OnePerNode() && (node_rank != MASTER) )
            continue;

        char* plugpath[1];
        plugpath[0]=new char[256];
        strcpy(plugpath[0],options.path.c_str());
        bool is_ok = inspector.Collect(1, plugpath);
        
        if (!is_ok and Rank==MASTER) LOG_ERROR << "Failed loading data for plugin " << inspector.Name();
        

        // populate json using type specific information.
        Value inspObj(kObjectType);
      
        Items data = inspector.GetItems();
        // collect and store strings
        for (auto n : data.strings) {
          Value fieldObj(kObjectType);
          if(PRINTMATCH) fieldObj.AddMember("match", n.match(), allocator);

          Value array(kArrayType);          
          for (auto m : n.value()) {
              LOG_DEBUG << "Loaded data: " << n.name() << " -> " << m;
              Value s(m.c_str(), allocator);
              array.PushBack(s, allocator);
          }
          fieldObj.AddMember("value", array , allocator);
          
          Value k(n.name().c_str(), allocator);
          inspObj.AddMember(k, fieldObj , allocator);
        }
        // collect and store floats
        for (auto n : data.floats) {
          Value fieldObj(kObjectType);
          if(PRINTMATCH) fieldObj.AddMember("match", n.match(), allocator);
          Value s(n.unit().c_str(), allocator);
          fieldObj.AddMember("unit",s , allocator);

          Value array(kArrayType);          
          for (auto m : n.value()) {
              LOG_DEBUG << "Loaded data: " << n.name() << " -> " << m;
              array.PushBack(m, allocator);
          }
          fieldObj.AddMember("value", array , allocator);
          
          Value k(n.name().c_str(), allocator);
          inspObj.AddMember(k, fieldObj , allocator);
        }
        // collect and store ints
        for (auto n : data.integers) {
          Value fieldObj(kObjectType);
          if(PRINTMATCH) fieldObj.AddMember("match", n.match(), allocator);
          Value s(n.unit().c_str(), allocator);
          fieldObj.AddMember("unit",s , allocator);


          Value array(kArrayType);          
          for (auto m : n.value()) {
              LOG_DEBUG << "Loaded data: " << n.name() << " -> " << m;
              array.PushBack(m, allocator);
          }
          fieldObj.AddMember("value", array , allocator);
          
          Value k(n.name().c_str(), allocator);
          inspObj.AddMember(k, fieldObj , allocator);
        }
        // collect and store bools
        for (auto n : data.bools) {
          Value fieldObj(kObjectType);
          if(PRINTMATCH) fieldObj.AddMember("match", n.match(), allocator);
          Value s(n.unit().c_str(), allocator);
          fieldObj.AddMember("unit",s , allocator);


          Value array(kArrayType);          
          for (auto m : n.value()) {
              LOG_DEBUG << "Loaded data: " << n.name() << " -> " << m;
              array.PushBack(m, allocator);
          }
          fieldObj.AddMember("value", array , allocator);
          
          Value k(n.name().c_str(), allocator);
          inspObj.AddMember(k, fieldObj , allocator);
        }
        // add all entries for this inspector
        if(!inspObj.ObjectEmpty()){
            Value k(inspector.Name().c_str(), allocator);
            procDoc.AddMember(k, inspObj, allocator);
        }
    }
    
    
    
    for(vector<Info*>::iterator iter = inspectors.begin(); iter != inspectors.end(); ++iter) {
        delete *iter;
    }


    /* Start data gathering at node level. The master of each node 
     * collects all the information of the node, only if something
     * was written
     */
    
    
    
    Document nodeDoc;    // document is the root of a json message
    
    nodeDoc.SetObject(); // define the document as an object (instead of array)

    if (node_rank == MASTER) {
        Document::AllocatorType& allocator = nodeDoc.GetAllocator();    
        
        Value vname(NodeNameStr.c_str(), allocator);
        nodeDoc.AddMember("NodeName", vname, allocator);
        

        Value procs(kArrayType);        
        procs.PushBack(procDoc.GetObject(), allocator);

        int length;
        MPI_Status status;
        
        for (int i=1; i < node_size; i++) {
            LOG_VERBOSE << "Receiving data from node rank " << i;
            MPI_Recv(&length, 1, MPI_INT, i, 1, node_comm, &status);
            if (length == 0) continue;
            char* rec_buf;
            rec_buf = (char *) malloc(length+1);
            MPI_Recv(rec_buf, length+1, MPI_CHAR, i, 1, node_comm, &status);
            int temp_rank=0;
            MPI_Recv (&temp_rank, 1, MPI_INT, i, 1, node_comm, &status);

            Document dn;
            dn.Parse(rec_buf);
            
            Value vv(kObjectType);
            vv.CopyFrom(dn, allocator);
            procs.PushBack(vv, allocator);
            free(rec_buf);
            
        }
        nodeDoc.AddMember("ProcList", procs, allocator);

        
    } else {
        if (procDoc.ObjectEmpty()) {
            int len=0;
            MPI_Send(&len, 1, MPI_INT, MASTER, 1, node_comm);
        } else {
            StringBuffer sb;
            Writer<rapidjson::StringBuffer> writer(sb);
            procDoc.Accept(writer);

            std::string s = sb.GetString();
            LOG_DEBUG << "Json dump of Rank " << Rank << ": " << s;
            int len = s.length();
            char* schar=new char[len+1];
            strcpy(schar, s.c_str() );
            MPI_Send(&len, 1, MPI_INT, MASTER, 1,node_comm);
            MPI_Send(schar, s.length()+1 , MPI_CHAR, MASTER, 1, node_comm);

            delete [] schar;
            MPI_Send(&Rank, 1 , MPI_INT, MASTER, 1, node_comm);
            
        }
    }
    
    MPI_Barrier(node_comm);

    /* The master of the master node collects all the information
     * and dumps the file.
     */
    if (master_rank == MASTER && node_rank == MASTER) {
        // document is the root of a json message
        Document *outDoc;
        outDoc=new Document;
        // define the document as an object rather than an array
        outDoc->SetObject();
        Document::AllocatorType& allocator = outDoc->GetAllocator();
        
        // Add comment
        Value s(options.comment.c_str(), allocator);
        outDoc->AddMember("Comment", s, allocator);
        
        // Add time
        time_t t = time(0);   // get time now
        struct tm * now = localtime( & t );
        string time_date;
        time_date = to_string(now->tm_year + 1900) + "-" + to_string(now->tm_mon + 1) + "-"
                  + to_string(now->tm_mday) + " "
                  + to_string(now->tm_hour) + ":"
                  + to_string(now->tm_min)  + ":"
                  + to_string(now->tm_sec);
        Value time(time_date.c_str(), allocator);
        outDoc->AddMember("DateTime", time, allocator);
        
                
        // Add object for this node to the final output json file
        
        Value nodes(kArrayType);        
        nodes.PushBack(nodeDoc.GetObject(), allocator);
        
        int length;
        MPI_Status status;
        for (int i=1; i < master_size; i++) {
            MPI_Recv(&length, 1, MPI_INT, i, 1, master_comm, &status);
            if (length == 0) continue;
            char* rec_buf;
            rec_buf = (char *) malloc(length+1);
            MPI_Recv(rec_buf, length+1, MPI_CHAR, i, 1, master_comm, &status);
            
            // parse json received from another node and add it to output

            MPI_Recv(&length, 1, MPI_INT, i, 1, master_comm, &status);
            if (length == 0) continue;
            char* NodeName_buf;
            NodeName_buf = (char *) malloc(length+1);
            MPI_Recv (NodeName_buf, length+1, MPI_CHAR, i, 1, master_comm, &status);

            Document dn;
            dn.Parse(rec_buf);
            Value kk(NodeName_buf, allocator);
            Value vv(kObjectType);
            vv.CopyFrom(dn, allocator);

            nodes.PushBack(vv, allocator);

            free(rec_buf);
            free(NodeName_buf);
        }
        
        outDoc->AddMember("NodeList",nodes , procDoc.GetAllocator() );


        end_time = std::chrono::system_clock::now();
        std::chrono::duration<double>diff_time = (end_time - start_time);
    
                // write prettified JSON to file
        Value ExecTime(kObjectType);
        outDoc->AddMember("ExecTime",(diff_time).count() , procDoc.GetAllocator() );
        
        StringBuffer sb;
        PrettyWriter<rapidjson::StringBuffer> writer(sb);
        outDoc->Accept(writer);
    
        std::ofstream o(options.output_filename.c_str());
        o << std::setw(4) << sb.GetString() << std::endl;
        o.close();
        LOG_INFO << "Output written to " << options.output_filename.c_str();
        
        
        
    } else if (node_rank == MASTER) {
        if (nodeDoc.ObjectEmpty()) {
            int len=0;
            MPI_Send(&len, 1, MPI_INT, MASTER, 1, master_comm);
        } else {
            /* Send node info (only if something is present) */
            StringBuffer sb;
            Writer<rapidjson::StringBuffer> writer(sb);
            nodeDoc.Accept(writer);

            std::string s = sb.GetString();
            LOG_DEBUG << "Json dump of Node " << master_rank << ": " << s;
            int len = s.length();
            char* schar=new char[len+1];
            strcpy(schar, s.c_str() );
            MPI_Send(&len, 1, MPI_INT, MASTER, 1, master_comm);
            MPI_Send(schar, len+1 , MPI_CHAR, MASTER, 1, master_comm);
            delete [] schar;
            
            len=NodeNameStr.length();
            schar=new char[len+1];
            strcpy(schar, NodeNameStr.c_str() );
            MPI_Send(&len, 1, MPI_INT, MASTER, 1, master_comm);
            MPI_Send(schar, len +1, MPI_CHAR, MASTER, 1, master_comm);
            delete [] schar;
        }
    }
    
    
    if (Rank==MASTER){           
        LOG_INFO << "Analysis completed.";  
    }
    
    
    
    
    
    
}


void ListPlugins(std::string plugin_path){
    
    
    vector<Info*> inspectors;

    auto plugin_system = PluginSystem();
    
    plugin_system.LoadInfoInstances(plugin_path, inspectors);
   
    std::cout << "Inspectors found in " << plugin_path << " :" << std::endl;
    
    for(vector<Info*>::iterator iter = inspectors.begin(); iter != inspectors.end(); ++iter) {
//         LOG_INFO.width(15);
        std::cout <<  "--> " << std::left << std::setw(15) << (*iter)->Name();
        std::cout <<  (*iter)->Description() <<  std::endl;
    }
    
    
    
}


bool CheckPlugins(std::string plugin_path, std::vector<std::string> required, int rank){
    
    if(required.size()>0){ 
        vector<Info*> inspectors;

        auto plugin_system = PluginSystem();
        
        plugin_system.LoadInfoInstances(plugin_path, inspectors);
    

        if(rank==MASTER) LOG_INFO << "Checking required inspectors in " << plugin_path ;
        bool found_all=1;
        for(std::vector<std::string>::iterator re_iter = required.begin(); re_iter != required.end(); ++re_iter){
            bool found=0;
            for(vector<Info*>::iterator iter = inspectors.begin(); iter != inspectors.end(); ++iter) {
                if (FindCaseInsensitive((*iter)->Name(), *re_iter) != std::string::npos){
                    found=1;
                }
            }
            if(!found){
                found_all=0;
                if(rank==MASTER) LOG_ERROR << "Required plugin " << *re_iter << " not found." ; 
            }
        }
    return found_all;
    }

    else return 1;
    
}




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




