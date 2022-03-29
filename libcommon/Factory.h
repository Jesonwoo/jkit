#ifndef FACTORY_H
#define FACTORY_H
#include "Singleton.h"
#include <map>
#include <queue>
template <typename Id, typename T, class C>
class Factory:public Singleton<C>
{
    using ID=Id;
    using Type=T;
    using Class=C;
    using CreatorFunc=Type*(*)(void);

public:
    Type* create(const ID & id);
    bool registerCreatorFunc(const ID & id, const CreatorFunc & func);
    // Avoid cross-DLL delivery crashes caused by different CRT links
    // so name must be literal
    bool registerCreatorName(const ID & id, const char * name);
    bool unregisterCreatorFunc(const ID & id);
    bool unregisterCreatorName(const ID & id);
    const char * name(const ID & id);

private:
    std::map<ID, const char*> m_nameMap;
    std::map<ID, CreatorFunc> m_creatorMap;
    std::queue<ID> m_ids;
};

#endif // FACTORY_H
