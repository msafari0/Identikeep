#pragma once
#include <pugg/Driver.h>
#include <list>
#include <utility> 
#include <string>
#include <memory>

//Log match enum
namespace match
{
typedef enum
{
    none = 1 << 0,
    list = 1 << 1,
    node = 1 << 2,
    world = 1 << 3
} MatchEnum;
inline MatchEnum operator|(MatchEnum a, MatchEnum b)
{return static_cast<MatchEnum>(static_cast<int>(a) | static_cast<int>(b));}
}

template< typename T >
class Item
{
public:
    Item(const std::string name, const match::MatchEnum match, const std::vector<T> value)
      : m_name(name), m_unit(""), m_match(match), m_value(value) {};
    Item(const std::string name, const match::MatchEnum match, const T value)
      : m_name(name), m_unit(""), m_match(match) {m_value.push_back(value);};
    Item(const std::string name, const std::string unit, const match::MatchEnum match, const std::vector<T> value)
      : m_name(name), m_unit(unit), m_match(match), m_value(value) {};
    Item(const std::string name, const std::string unit, const match::MatchEnum match, const T value)
      : m_name(name), m_unit(unit), m_match(match) {m_value.push_back(value);};
    ~Item() {};
    std::string name(){return m_name;};
    std::string unit(){return m_unit;};
    std::vector<T> value(){return m_value;};
    int match(){return int(m_match);};
    
private:
    std::vector<T> m_value;
    std::string m_name;
    std::string m_unit;
    int m_match;
};

/* This is quite ugly, but allows to improve this implementatin in the future
 * 
 * 
 * 
 */
class Items
{
public:
    std::vector< Item<int> > integers;
    std::vector< Item<std::string> > strings;
    std::vector< Item<float> > floats;
    std::vector< Item<bool> > bools;
};



class Info
{
public:
    Info() {}
    virtual ~Info() {}

    virtual bool Collect(int argc, char *argv[]) =0;
    virtual Items GetItems() =0;
    virtual std::string Name() =0;
    virtual std::string Description() =0;
    
    virtual bool FreeText() = 0;
    virtual bool OnePerNode() = 0;

    static const int version = 1;
    static const std::string server_name() {return "InfoServer";}

};


class InfoDriver : public pugg::Driver
{
public:
    InfoDriver(std::string name, int version) : pugg::Driver(Info::server_name(), name, version) {}
    virtual Info* create() = 0;
};
