#include "Factory.h"
#include <algorithm> // std::find
template<typename Id, typename T, class C>
typename Factory<Id, T, C>::Type *Factory<Id, T, C>::create(const ID &id)
{
    auto itr = m_creatorMap.find(id);
    if(itr == m_creatorMap.end()) {
        return nullptr;
    }
    return (itr->second)();
}

template<typename Id, typename T, class C>
bool Factory<Id, T, C>::registerCreatorFunc(const ID &id, const char *name, const CreatorFunc &func)
{
    if(contain(id)) {
        return false;
    }
    m_nameMap.insert(id, name);
    m_creatorMap.insert(id, func);
    return true;
}

template<typename Id, typename T, class C>
bool Factory<Id, T, C>::unregisterCreatorFunc(const ID &id)
{
    if(!contain(id)) {
        return true;
    }

    m_creatorMap.erase(id);
    m_nameMap.erase(id);
    return true;
}

template<typename Id, typename T, class C>
bool Factory<Id, T, C>::contain(const ID &id)
{
    return m_creatorMap.find(id) != m_creatorMap.end();
}

template<typename Id, typename T, class C>
const char *Factory<Id, T, C>::name(const ID &id)
{
    if(contain(id)) {
        return m_nameMap.at(id);
    }
    return nullptr;
}

template<typename Id, typename T, class C>
typename Factory<Id, T, C>::ID Factory<Id, T, C>::id(const char *name)
{
    for(auto itr = m_nameMap.begin(); itr != m_nameMap.end(); ++itr) {
        if(std::string(itr->second) == std::string(name)) {
            return itr->first;
        }
    }
    return ID();
}

