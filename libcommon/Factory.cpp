#include "Factory.h"

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
bool Factory<Id, T, C>::registerCreatorFunc(const ID &id, const CreatorFunc &func)
{

}

template<typename Id, typename T, class C>
bool Factory<Id, T, C>::registerCreatorName(const ID &id, const char *name)
{

}

template<typename Id, typename T, class C>
bool Factory<Id, T, C>::unregisterCreatorFunc(const ID &id)
{

}

template<typename Id, typename T, class C>
bool Factory<Id, T, C>::unregisterCreatorName(const ID &id)
{

}

template<typename Id, typename T, class C>
const char *Factory<Id, T, C>::name(const ID &id)
{

}

