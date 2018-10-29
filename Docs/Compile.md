# Compile

In order to compile cmake >= 3.0.2 is needed. On some old system which 
do not have recent STL libraries, Boost is used as replacement.
If available, compile with boost.
In order to do so specify the following environment variables:

    export BOOST_INCLUDE_DIR=/absolute/path/to/boost/include
    export BOOST_LIBRARY_DIR=/absolute/path/to/boost/lib
    export BOOST_ROOT=/absolute/path/to/boost
    

# Compile Squidly on some machines

## Marconi with IntelMPI+MKL

    ./prepare.sh
    module load autoload boost mkl intelmpi cmake
    mkdir -p build && cd build
    export BOOST_INCLUDE_DIR=$BOOST_INCLUDE
    export BOOST_ROOT=$BOOST_HOME
    export BOOST_LIBRARY_DIR=$BOOST_LIB
    
    cmake -DDESC=intel_intelmpi_mkl -DRE_PROVIDER=boost -DCMAKE_BUILD_TYPE=Release ..
    make

## Galileo with IntelMPI+MKL

    ./prepare.sh
    module load autoload intelmpi/2017--binary boost/1.58.0--intelmpi--5.0.2--binary mkl cmake
    mkdir -p build && cd build
    export BOOST_INCLUDE_DIR=$BOOST_INCLUDE
    export BOOST_ROOT=$BOOST_HOME
    export BOOST_LIBRARY_DIR=$BOOST_LIB
    
    cmake -Wno-dev -DDESC=intel_intelmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make

## Galileo with gnu+openMPI+MKL

    ./prepare.sh
    module load autoload openmpi/1.8.5--gnu--4.9.2 boost/1.57.0--gnu--4.9.2 mkl cmake
    mkdir -p build && cd build
    export BOOST_INCLUDE_DIR=$BOOST_INCLUDE
    export BOOST_ROOT=$BOOST_HOME
    export BOOST_LIBRARY_DIR=$BOOST_LIB
    
    cmake -Wno-dev -DDESC=gnu_openmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make
    
## Galileo with pgi+openMPI+MKL

    ./prepare.sh
    module load autoload pgi/17.5 boost/1.57.0--gnu--4.9.2 mkl cmake
    mkdir -p build && cd build
    export BOOST_INCLUDE_DIR=$BOOST_INCLUDE
    export BOOST_ROOT=$BOOST_HOME
    export BOOST_LIBRARY_DIR=$BOOST_LIB
    
    cmake -Wno-dev -DCMAKE_CXX_COMPILER=pgc++ -DDESC=gnu_openmpi_mkl -DCMAKE_BUILD_TYPE=Release ..
    make


## Piz Daint with PrgEnv-Intel

    module switch PrgEnv-cray PrgEnv-intel
    ./prepare.sh
    cd re2-2017-12-01
    mkdir build
    cd build/
    cmake -DCMAKE_INSTALL_PREFIX=../install -DBUILD_SHARED_LIBS=ON ..
    make
    make install
    # store install path
    cd ../install
    export RE2_INSTALL_DIR=`pwd`
    cd ../../
    
    
    mkdir -p build && cd build
    cmake -DDESC=intel_intelmpi_mkl -DCMAKE_CXX_COMPILER=CC -DCMAKE_CXX_FLAGS="-shared -dynamic" \
          -DRE_PROVIDER=re2 -DRE2_INCLUDE=${RE2_INSTALL_DIR}/include/ -DRE2_LIB=${RE2_INSTALL_DIR}/lib/  ..
    make

## Piz Daint with PrgEnv-gnu

    module switch PrgEnv-cray PrgEnv-intel
    ./prepare.sh
    cd re2-2017-12-01
    mkdir build
    cd build/
    cmake -DCMAKE_INSTALL_PREFIX=../install -DBUILD_SHARED_LIBS=ON ..
    make
    make install
    # store install path
    cd ../install
    export RE2_INSTALL_DIR=`pwd`
    cd ../../
    
    
    mkdir -p build && cd build
    cmake -DDESC=intel_intelmpi_mkl -DCMAKE_CXX_COMPILER=CC -DCMAKE_CXX_FLAGS="-shared -dynamic" \
          -DRE_PROVIDER=re2 -DRE2_INCLUDE=${RE2_INSTALL_DIR}/include/ -DRE2_LIB=${RE2_INSTALL_DIR}/lib/  ..
