# Identikeep

Identikeep is a plugin based tool to collect information on the hardware and 
the software implementation of HPC clusters.
It is written in C++11 and it uses MPI to collect the information.

## Program guidelines and requirements

The main requirements for the development of Identikeep are:

  * Compatibility with recent versions of most widespread compilers for scientific software.
  * Minimal dependencies.
  * Cross-platform

As of today, Identikeep can be compiled with the following compilers:

  * GNU > 4.8
  * Intel > 15
  * PGI > 17
  * Cray compilers

**Important note:** Identikeep is in principle designed to run on both Windows and Unix-like operative systems. However, the most part of the software plugins are specifically designed for operative systems that follow the POSIX standard.


## Compilation

See [Docs/Compile.md](Docs/Compile.md)

## Program description

### Quick start

Identikeep is supposed to be run within the same environment used for the program to be benchmarked.
After having prepared the environment with the appropriate modules and 
environmental variables, just run the `identikeep` program with the standard `mpirun`
or `srun` interfaces.

By default the program tries to load all the plugins that it finds and 
it collects information from all plugins that were successfully loaded.
This has approach has the advantage of allowing for plugin to fail at runtime.
A typical example is when profiling a program using different implementations
of the BLAS library. As it is customary in clusters configuration, BLAS libraries
will reside in different, non standard, directories that have to be loaded
with modules.
Therefore only the plugin associated with that specific implementation of the BLAS
routines will be successfully loaded at runtime while the other will fail 
without impacting on the execution of the whole program.

By default, `identikeep` searches for the plugins in the path from which it was launched: a different directory can be specified with the option 
`-p`. To specify a list of plugins that must provide information just specify their names proceeded by `[`:

    mpirun ./identikeep -p=PATH/TO/PLUGINS [CPU [Mem [OS
    
names a case insensitive. An error will be issued if any of these plugins
fails to load.
A file named `sysinfo.json` containing al the information collected is created
in the current working directory.

For a full list of the available options, run:

    ./identikeep --help

### Options
Here is a full list of `identikeep` options:

-   `-ls` : list available inspectors at the given PLUGIN_PATH to stdout.
-   `-p=/path` or `--plugins=/path` : set plugins path.
-   `-c="..."` or `--comment=".."` : add a comment (free text in general) to the output file.
-   `-t="..."` or `--tag=".."` : add a tag to the output file name.
-   `[PLUGIN_NAME` : require the plugin `PLUGIN_NAME`. Execution will abort if the plugin `PLUGIN_NAME` is not found
-   `]PLUGIN_NAME` : exclude the plugin `PLUGIN_NAME` from the analysis, if present among the plugins.

-   `--trace` : print everything the code does.
-   `--debug` : print debug info.
-   `--verbose` : print debug info.

## Output

Identikeep system analysis is stored in the output file `sysinfo.json` (or `sysinfo_TAG.json` if the option `--tag=TAG` is provided).
The output format is [.json](json.org), and contains the information collected on each computing node.
Plugins are loaded by each MPI process: depending on the plugin implementation, a single plugin may be executed only once per physical computing node, or by each MPI process.

#### Output file example
For example, it is possible to run `identikeep` on two computing nodes, with a total of 8 MPI processes (i.e. 4 for each computing node).
Let's imagine to have three installed plugins: `Plugin1` and `Plugin3` will run only once for each computing node, while `Plugin2` will be executed by each MPI process.
In this case, the output `sysinfo.json` will have the following structure:


    {
        "Comment": "MyComment",                 *File comment
        "DateTime": "yyyy-mm-dd hh:mm:ss",      *Time at which identikeep was executed
        "NodeList": [                           *List of physical nodes
            {
                "NodeName": "NODE_1_NAME",      *Name of the physical node
                "ProcList": [                   *List of processes on the node
                    {
                        "GlobalRank": 0,        *Global rank of the MPI process
                        "Plugin1": {             
                            ...                 *Results of Plugin1 computed by the master process
                        },
                        "Plugin2": {
                            ...                 *Results of Plugin2 computed by the master process
                        },
                        "Plugin3": {
                            ...                 *Results of Plugin3 computed by the master process
                        }
                    },
                    {
                        "GlobalRank": 1,        *Global rank of the MPI process
                        "Plugin2": {
                            ...                 *Results of Plugin2 computed by process 1. (Plugin1 and Plugin3 run one per node, and are executed only by the master of the node) 
                        }
                    },        
                    {
                        "GlobalRank": 2, 
                        "Plugin2": {                    
                            ...                 *Results of Plugin2 computed by process 2.
                        }
                    },  
                    {
                        "GlobalRank": 3, 
                        "Plugin2": {
                            ...                 *Results of Plugin2 computed by process 3.
                        }
                    }
                ]
            },
            {
                "NodeName": "NODE_2_NAME",      *Name of the physical node
                "ProcList": [                   *List of processes on the node
                    {
                        "GlobalRank": 4,        *Global rank of the MPI process
                        "Plugin1": {             
                            ...                 *Results of Plugin1 computed by process 4 (master of the node NODE_2_NAME)
                        },
                        "Plugin2": {
                            ...                 *Results of Plugin2 computed by process 4 (master of the node NODE_2_NAME)
                        },
                        "Plugin3": {
                            ...                 *Results of Plugin3 computed by process 4 (master of the node NODE_2_NAME)
                        }
                    },
                    {
                        "GlobalRank": 5,        *Global rank of the MPI process
                        "Plugin2": {
                            ...                 *Results of Plugin2 computed by process 5. 
                        }
                    },        
                    {
                        "GlobalRank": 6,        *Global rank of the MPI process
                        "Plugin2": {                    
                            ...                 *Results of Plugin2 computed by process 6.
                        }
                    },  
                    {
                        "GlobalRank": 7,        *Global rank of the MPI process
                        "Plugin2": {
                            ...                 *Results of Plugin2 computed by process 7.
                        }
                    }
                ]
            },
        ],
        "ExecTime": xx.xxxx                   *Execution time of the analysis (in seconds)
    }
    
### Log file
Depending on the verbosity level provided as option, identikit produces log files. 
If the log level is not speified (i.e. with `--trace`), only some infos are written by the master node on a log file named `sysinfo(_TAG).json.log`.
If a deeper level for log is required, each MPI process writes its own log file in `sysinfo(_TAG).json.log-N`, where `N` is the global rank of the process.
        

## List of available plugins
Identikeep comes with some plugins, whose role is to inspect different features of the cluster.
Here is a list of them.

#### CPU

- **Name:** CPU
- **Library name:** libInfoCPU

The CPU plugin provides information on the CPU hardware. In particular, the following fields are created in the output file:

- **cpuModel:**     Model of the installed CPUs (list)
- **cpuMhz:**       Detected CPUs frequency (list)
- **cpuCache1:**    CPU L1 cache size
- **cpuCache2:**    CPU L2 cache size
- **cpuCache3:**    CPU L3 cache size

The plugin runs only once per computing node.
**The plugin runs only once per computing node.**


#### CMPI

- **Name:** CMPI
- **Library name:** libInfoMPI

The CMPI plugin provides information about the Message Passing Interface library installed on the system. The following fields are created in the output file:

- **mpiVersion:**     MPI version
- **mpiSubversion:**  MPI subversion

**The plugin runs only once per computing node.**


#### Env

- **Name:** Env
- **Library name:** libInfoEnv

The Env plugin provides information about the environment variables defined on the system. The following fields are created in the output file:

- **envVariables:**   List of environment variables and their value

**The plugin runs only once per computing node.**


#### MEM

- **Name:** MEM
- **Library name:** libInfoMEM

The MEM plugin provides information about the system memory. The following fields are created in the output file:

- **memTotal:**   Amount of installed memory

**The plugin runs only once per computing node.**

#### OMP

- **Name:** OMP
- **Library name:** libInfoOMP

The OMP plugin provides information about the OpenMP library installed on the system. The following fields are created in the output file:

- **openmpVersion:**            Version of the installed OpenMP library (yyyymm)
- **openmpNumThreads:**         return value of omp_get_num_threads function
- **openmpMaxThreads:**         return value of omp_get_max_threads function
- **openmpThreadLimit:**        return value of omp_get_thread_limit function
- **openmpNumProcs:**          return value of omp_get_num_procs() function
- **openmpNested:**            return value of omp_get_nested() ) function
- **openmpMaxActiveLevels:**   return value of omp_get_max_active_levels() ) function


#### OS

- **Name:** OS
- **Library name:** libInfoOS

The OS plugin provides information about the Operative System installed on the node. The following fields are created in the output file:

- **osVersion:**    Version on the installed OS

**The plugin runs only once per computing node.**


#### Libs

- **Name:** Libs
- **Library name:** libInfoLibs

The Libs plugin provides information about the shared libraries loaded by the executable at runtime. The following fields are created in the output file:

- **libsLoaded:**    List of the loaded libraries

**The plugin runs only once per computing node.**


#### Exec

- **Name:** Exec
- **Library name:** libInfoExec

The Exec plugin provides information about the executable. The following fields are created in the output file:

- **execPath:**         Absolute path to the executable
- **execSize:**         Size of the executable
- **execLastModified:**  Date of the executable last modification (or creation)

**The plugin runs only once per computing node.**


#### FS

- **Name:** FS
- **Library name:** libInfoFS

The FS plugin provides information concerning the filesystem. The following fields are written to the output file:

- **fsType:**           Filesystem type (e.g. ext3, ext4, gpfs, nfs)
- **fsBlockSize:**      Block size of the filesystem

**The plugin runs only once per computing node.**

#### MPIBench

- **Name:** MPIBench
- **Library name:** libMPIBench

The MPIBench plugin is actually a very tiny and non-intrusive benchmark, which provides some info about the quality of MPI communications.
In order to keep the execution time low, only communications to and from the master node are considered. For this reason, even if the plugin is executed by each MPI process, only the node masters will provide information.
The following fields are created in the output file:

- **mpibenchBandwidthBcast:**        MPI Broadcast bandwidth, achieved when bradcasting from the master node. Value is written only by the global master.
- **mpibenchPingPong:**             Ping pong with the global master node. Value is written only by the masters of each node, except the global master.
- **mpibenchBandwidthPingPong:**    Ping pong bandwidth with the global master node. Value is written only by the masters of each node, except the global master.



#### CUDA

- **Name:** CUDA
- **Library name:** libInfoCUDA

The CUDA plugin provides information about the NVIDIA GPUs installed on the node and accessible by the host. Information is collected via the CUDA library. The following fields are created in the output file:

- **cudaDevice:**       List of the names of the detected GPUs
- **cudaVersion:**      CUDA version for each detected device (Major.minor)
- **cudaClock:**        Clock frequency of GPU cores for each device
- **cudaTotMem:**       Total installe memory for each GPU

**The plugin runs only once per computing node.**


### Adding plugins
There are two ways to implement new plugins. The first (raccomended) one is to write it following the template in the folder `Plugins/Template`, ant then compile it as a shared module.

A second easier but less efficient and less powerful way is to write a stand-alone program, e.g. `myextension.sh`, and place it in a folder named `extensions` inside the plugin `bin` folder.
All the programs inside the `extensions` folder will be executed by the **Extensions** plugin. Each extension **must write the following values in the standard output**:

- Field name:

        Name: field_name

- Value type, which can be `string`, `integer` or `float`

        Type: value_type

- Value, whose form depends on the Type:

        Value: field_value
    
- Value unit (optional)

        Unit: field_unit

- Log message (optional), reported by identikeep log file

        Message: my_message
        
Value is the only field that can be repeated. In this case, all the values will be saved in the output file as a list.
For example, the following bash script collects the information on the CPU frequencies:

    #!/bin/bash
    echo "Name: MyCpuFreq"
    echo "Type: float"

    array=(`cat /proc/cpuinfo | sed  -rn 's/[cC]pu MHz[ \t]+:[ \t]+([[:digit:]]+.[[:digit:]]+)/\1 /gp'`)
    ncores=${#array[@]}

    for i in $(seq 0 $(($ncores-1)))
    do
            echo Value: ${array[i]}
    done

    echo "Unit: Mhz"

If this script is placed in the foled `bin/extensions`, a field named `MyCpuFreq` will be created in the plugin `Extensions`, which will contains a list of the CPU frequencies.

This approach clearly has some limitations:

- Extensions cannot access the parameters concerning MPI, such that they cannot make use of MPI calls. So, extensions can be used only to collect **local** information. 
- Moreover, it is required that each extension is able to run on each node.
- At the current implementation, each extension can return only one field (this fact may change in the future)

The main advantages are, instead:

- Extensions are much simpler to write 
- The user is free to code the extensions in any language (bash, python, C/C++, Fortran, etc.)



## Advanced use
Identikeep provides a tool to intercept the MPI calls of an executable via the PMPI interface. If we need to run Identikeep to have a snapshot of the system configuration when a program `MyProg` is launched, there are two different ways.

The first one, let's call it the _standard way_, is to run `identikeep` just before `MyProg`, i.e.:

    mpirun ./identikeep [OPTIONS]
    mpirun ./MyProg

The main issue concerning this approach is that we must ensure that both `identikeep` and `MyProg` will run on the same computing nodes and (desirably) the same libraries are loaded.

A second way is to exploit a library provided by the Identikeep package, `libMPIntercept`, to perform the `identikeep` analysis at `MyProg` runtime.
This is possible by exporting the LD\_PRELOAD environment variable before running `MyProg`.
In this case, Identikeep options are passed via environment variables. Here is a list of these variables:

- `IDENTIKIT_PATH` : set the path of the plugins (equivalent to the `-p` option of the _standard use_).
- `IDENTIKIT_COMMENT` : add a comment to the output file (equivalent to the `-c` option of the _standard use_).
- `IDENTIKIT_TAG` : add a tag to the output file name (equivalent to the `-t` option of the _standard use_).
- `IDENTIKIT_LOG=verbose/debug/trace` : Set log level that will be written in the log file
- `IDENTIKIT_BLACKLIST="plug1 plug2 ..."` : list of plugins to be blacklisted (i.e. they won't be executed during the analysis, even if present in the plugin path).
- `IDENTIKIT_REQUIRE="plug1 plug2 ..."`: list of plugins that must be executed (execution will fail if the plugins are not present in the plugin path).

#### Example
These commands

    mpirun ./identikeep -c="My comment" -p=My/Plugin/Path -t="Mytag" --verbose ]OMP [Mem [CPU 
    mpirun ./MyProg

should provide an output similar to:

    export IDENTIKIT_COMMENT="My comment"
    export IDENTIKIT_TAG="Mytag"
    export IDENTIKIT_LOG=verbose
    export IDENTIKIT_BLACKLIST="OMP"
    export IDENTIKIT_REQUIRE="Mem CPU"
    mpirun -x LD_PRELOAD=My/Plugin/Path/libMPIntercept.so ./MyProg

    
#### Requirements 
In order to use this _runtime analysis_ feature, `MyProg` **must be written in C/C++** and **must make use of MPI** (i.e. it must call `MPI_Init` and `MPI_Finalize`)


## Identiview tool
As said before, information collected by `identikeep` is written in a `.json` file, which can be easily interpreted. Many tools for representing `.json` data 
in a nice format are available online.
However, it may result hard to have a quick view on the collected data if `identikeep` has been executed on many nodes. For this reason, inside the Identikeep package, a tiny parsing tool, called `identiview` and written in Python 2, is available

### Requirements
`identiview` is automatically installed, along with `identikeep` and its plugins, in the `bin` directory, only if the `python` 2.7 interpreter is detected on the system.

### Description
`identiview` is a simple tool to inspect the `identikeep` output, written in a `.json` file. Its aim is to visualize specific system information in a more human-readable way. Some examples of its use follows.

### Examples

Assuming to have the `identikeep` analysis written in the file `sysinfo.json`, here are some examples:


-   `./identiview sysinfo.json`

prints out the whole content of `sysinfo.json`, in a more human-readable format


-   `./identiview sysinfo.json --list-nodes`
-   `./identiview sysinfo.json -ln`

lists the nodes on which `identikeep` was executed, providing also the ranks of the processes that run on each of them.


-   `./identiview sysinfo.json --list-fields`
-   `./identiview sysinfo.json -lf`

lists list all the fields (i.e. the properties of the system) that appears in the `sysinfo.json` file (`openmpVersion`, `cpuMhz` etc.).


-   `./identiview sysinfo.json --node NODENAME`
-   `./identiview sysinfo.json -n NODENAME`

restrict the analysis to the node NODENAME.


-   `./identiview sysinfo.json --field FIELDNAME`
-   `./identiview sysinfo.json -f FIELDNAME`

restrict the analysis to the field FIELDNAME.


-   `./identiview sysinfo.json --rank RANK`
-   `./identiview sysinfo.json -r RANK`

restrict the analysis to the process with rank RANK.

These last three options (i.e. `-f`, `-r` and `-n`) can be combined together or with `-lf` and `-ln`. 
Moreover, `-f`, `-r` and `-n` accepts multiple arguments, separated by commas (",")

For example, to have a list of the plugins that were executed by process with rank 134, just type:

    ./identiview sysinfo.json -r 134 -lf

Moreover, to view the version of the OpenMP library loaded by the processes on node `r103c04s03` and `r103c04s04`, run:

    ./identiview sysinfo.json -n r103c04s03,r103c04s04 -f openmpVersion

For a summery of the described options, just type `./identiview --help`




## Identirun tool (experimental)

As long as `identikeep` was conceived to run coupled with an other executable (for example a benchmark `mybenchmark`), the user must ensure that both `identikeep` and `mybenchmark` run with the same MPI configuration.
For example, an practical way to do this could be

    mpirun MPIRUN_OPTIONS identikeep IDENTIKEEP_OPTIONS
    mpirun MPIRUN_OPTIONS mybenchmark MYBENCHMARK_OPTIONS

This approach is somehow redundant and may lead to mistakes. 

In order to simplify the execution of `identikeep`, a python-based tool named `identirun` is provided inside the Identikeep package. As the name suggests, it is a wrapper to `mpirun` which executes `identikeep` just before the main executable.
For example, running:

    identirun MPIRUN_OPTIONS mybenchmark MYBENCHMARK_OPTIONS

should be equivalent to call

    mpirun MPIRUN_OPTIONS identikeep -t=DATE_TIME
    mpirun MPIRUN_OPTIONS mybenchmark MYBENCHMARK_OPTIONS

The use of `identirun` doesn't allow to specify the `identikeep` options `IDENTIKEEP_OPTIONS`, such that the default options for `identikeep` are used.
The output file produced by `identikeep` invoked by `identirun` has a suffix which indicates the main executable name and the date and time of the execution.
For example, if the following command is executed at 14:35:12 of November 15, 2018

    identirun -np 4 mybenchmark

the output file name will be 

    sysinfo_mybenchmark_20181115_143512.json
    
**NOTE:** `identirun` is currently under development








