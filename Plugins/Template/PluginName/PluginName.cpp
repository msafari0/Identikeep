#include "PluginName.h"

// Logging
#include "spdlog/spdlog.h"
namespace spd = spdlog;


bool PluginName::Collect(int argc, char *argv[])
{
    auto log = spd::get("console");
    bool is_ok = true;

    log->info("Parsing {} info...", this->Name());
    
    /* stores a relevant values to be saved */
    int v = 9;
    
    Item<int> i = Item<int>("SomeVariable",  // name of the variable
                              match::list | match::node | match::world, // whether the information should be the same on all nodes
                              v ); // the actual value in this call
    /* append the item */
    m_items.integers.push_back(i);

    return is_ok;
};
