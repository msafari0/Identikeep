#include <mkl.h>
#include "MKL.h"

// Logging
#include "plog/Log.h"


bool MKL::Collect(int argc, char *argv[])
{
    bool is_ok = true;

    LOG_VERBOSE << "Parsing " << this->Name() << " info...";
    
     
    int v = INTEL_MKL_VERSION;
    Item<int> i = Item<int>("mklVersion", match::list | match::node | match::world, v );
    m_items.integers.push_back(i);


    return is_ok;
};
