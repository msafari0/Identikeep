#include <omp.h>
#include "OMP.h"

// Logging
#include "plog/Log.h"

#define MATCHALL match::list | match::node | match::world

bool OMP::Collect(int argc, char *argv[])
{
    bool is_ok = true;

    LOG_INFO << "Parsing " << this->Name() << " info...";
    
    /* From specs
     * The _OPENMP macro name is defined by OpenMP-compliant implementations as the
     * decimal constant yyyymm, which will be the year and month of the approved
     * specification. This macro must not be the subject of a 
     * #define or a #undef preprocessing directive.
     * 
     * {200505,"2.5"},{200805,"3.0"},{201107,"3.1"},{201307,"4.0"},{201511,"4.5"};
     */
     
    int v = _OPENMP;
    Item<int> i = Item<int>("openmpVersion", MATCHALL, v );
    m_items.integers.push_back(i);
    
    i = Item<int>("openmpNumThreads", MATCHALL, omp_get_num_threads() );
    m_items.integers.push_back(i);
    
    i = Item<int>("openmpMaxThreads", MATCHALL, omp_get_max_threads() );
    m_items.integers.push_back(i);
    
    i = Item<int>("openmpThreadLimit", MATCHALL, omp_get_thread_limit() );
    m_items.integers.push_back(i);
    
    i = Item<int>("openmpNumProcs", MATCHALL, omp_get_num_procs() );
    m_items.integers.push_back(i);
    
    i = Item<int>("openmpNested", MATCHALL, omp_get_nested() );
    m_items.integers.push_back(i);
    
    i = Item<int>("openmpMaxActiveLevels", MATCHALL, omp_get_max_active_levels() );
    m_items.integers.push_back(i);
    


    return is_ok;
};
