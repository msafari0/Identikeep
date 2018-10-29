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
        "NODE_1_NAME": {                        *Name of the first computing node
            "0 (0)": {                          *Rank of the MPI process on the node, while, in brakets, global rank of the process
                "Plugin1": {                   
                    ...                         *Results of Plugin1 computed by the master of the node
                },
                "Plugin2": {
                    ...                         *Results of Plugin2 computed by the master of the node
                },
                "Plugin3": {
                    ...                         *Results of Plugin3 computed by the master of the node
                }
            },
            "1 (1)": {
                "Plugin2": {
                    ...                         *Results of Plugin2 computed by process 1 (1). (Plugin1 and Plugin3 run one per node, and are executed only by the master of the node) 
                }
            },        
            "2 (2)": {
                "Plugin2": {                    
                    ...                         *Results of Plugin2 computed by process 2 (2).
                }
            },  
            "3 (3)": {
                "Plugin2": {
                    ...                         *Results of Plugin2 computed by process 3 (3).
                }
            }
        "NODE_2_NAME": {                        *The same is performed for the second computing node, where the master inside the node is the only one to execute Plugin1 and Plugin3
            "0 (4)": {
                "Plugin1": {
                    ...
                },
                "Plugin2": {
                    ...
                },
                "Plugin3": {
                    ...
                }
            },
            "1 (5)": {
                "Plugin2": {
                    ...
                }
            },        
            "2 (6)": {
                "Plugin2": {
                    ...
                }
            },  
            "2 (7)": {
                "Plugin2": {
                    ...
                }
            }
        },
        "Analysis execution time": 0.356                   *Execution time of the analysis (in seconds)
    }
    
### Log file
Depending on the verbosity level provided as option, identikit produces log files. 
If the log level is not speified (i.e. with `--trace`), only some infos are written by the master node on a log file named `sysinfo(_TAG).json.log`.
If a deeper level for log is required, each MPI process writes its own log file in `sysinfo(_TAG).json.log-N`, where `N` is the global rank of the process.
        

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



