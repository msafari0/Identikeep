#include <pugg/Kernel.h>

#include "CUDA.h"

#ifdef _WIN32
#  define EXPORTIT __declspec( dllexport )
#else
#  define EXPORTIT
#endif


extern "C" EXPORTIT void register_pugg_plugin(pugg::Kernel* kernel)
{
	kernel->add_driver(new CUDADriver());
}
