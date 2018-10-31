# Compile

In order to compile cmake >= 3.0.2 is needed. 

The following sequence of commands should work on most systems:

    ./prepare.sh
    mkdir -p build && cd build
    cmake ..
    make

If successuful, this procedure will create a directory `bin`, inside the `build` folder, which contains the plugins (with the prefix `lib`) and the `identikeep` executable.

## Compile Identikeep on some machines
**NOTE: the following procedures still have to be tested**

### Marconi with IntelMPI+MKL

    ./prepare.sh
    module load autoload boost mkl intelmpi cmake
    mkdir -p build && cd build
    
    cmake -DDESC=intel_intelmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make

### Galileo with IntelMPI+MKL

    ./prepare.sh
    module load autoload intelmpi/2017--binary boost/1.58.0--intelmpi--5.0.2--binary mkl cmake
    mkdir -p build && cd build
    
    cmake -Wno-dev -DDESC=intel_intelmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make

### Galileo with gnu+openMPI+MKL

    ./prepare.sh
    module load autoload openmpi/1.8.5--gnu--4.9.2 boost/1.57.0--gnu--4.9.2 mkl cmake
    mkdir -p build && cd build
    
    cmake -Wno-dev -DDESC=gnu_openmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make
    
### Galileo with pgi+openMPI+MKL

    ./prepare.sh
    module load autoload pgi/17.5 boost/1.57.0--gnu--4.9.2 mkl cmake
    mkdir -p build && cd build
    
    cmake -Wno-dev -DCMAKE_CXX_COMPILER=pgc++ -DDESC=gnu_openmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make


### Piz Daint with PrgEnv-Intel

    module switch PrgEnv-cray PrgEnv-intel
    ./prepare.sh    
    mkdir -p build && cd build
    
    export CRAYPE_LINK_TYPE=dynamic
    cmake -DDESC=intel_intelmpi_mkl -DCMAKE_CXX_COMPILER=CC ..
    make

### Piz Daint with PrgEnv-gnu

    module switch PrgEnv-cray PrgEnv-intel
    ./prepare.sh    
    mkdir -p build && cd build
    
    export CRAYPE_LINK_TYPE=dynamic
    cmake -DDESC=intel_intelmpi_mkl -DCMAKE_CXX_COMPILER=CC ..
