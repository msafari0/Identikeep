#include "CUDA.h"

// Logging
#include "plog/Log.h"


bool CUDA::Collect(int argc, char *argv[])
{
  bool is_ok = true;

  LOG_VERBOSE << "Parsing " << this->Name() << " info...";
  
  int nDevices;

  cudaGetDeviceCount(&nDevices);

  cudaDeviceProp prop[nDevices];

  std::vector<std::string> 	dev_list;
  std::vector<float> 	   	totalGlobalMem_list;
  std::vector<int> 		clockRate_list;
  std::vector<std::string> 	version_list;

  for (int i = 0; i < nDevices; i++){
	cudaGetDeviceProperties(&prop[i],i);

        dev_list.push_back(prop[i].name);
	totalGlobalMem_list.push_back(double(prop[i].totalGlobalMem)/1000000.);
	clockRate_list.push_back(prop[i].clockRate);
	
	std::string vv=std::to_string(prop[i].major)+"."+std::to_string(prop[i].minor);
	version_list.push_back(vv);
  }


     
    Item<std::string> idev_list = Item<std::string>("cudaDevice", match::list | match::node | match::world, dev_list);
    m_items.strings.push_back(idev_list);

    Item<std::string> iversion = Item<std::string>("cudaVersion", match::list | match::node | match::world, version_list);
    m_items.strings.push_back(iversion);

    Item<int> iclock = Item<int>("cudaClock", "KHz", match::list | match::node | match::world, clockRate_list);
    m_items.integers.push_back(iclock);

    Item<float> imem = Item<float>("cudaTotMem", "MB", match::list | match::node | match::world, totalGlobalMem_list);
    m_items.floats.push_back(imem);



    return is_ok;
};
