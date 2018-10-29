# FAQ

1. Why choose C++ for this project instead of Python?
   The intended usage of the program is (for example with slurm):
   
       srun /path/to/PCollect
       srun /path/to/program_to_benchmark
       
   this has the following implications:
   a) since the executable that collects the information must run in
   parallel on all nodes, MPI seems the most widespread choice.
   b) given a certain application, the data collection program should run
   with minimal to no additional dependencies that must be compatible with
   the compilation of the program to be benchmarked.
   
   Opting for a python program has the annoying consequence of asking the 
   user (or the sysadmin of the machine) to install MPI4PI with all the
   implementations of MPI that have and/or will be used to compile the
   program to be benchmarked.
   
2. Why this horrible plugin system?
   This not so nice indeed, but has the great advantage of isolating the
   development of the various plugins and isolate possible points of failures.
