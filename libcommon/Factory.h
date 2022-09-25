#ifndef FACTORY_H
#define FACTORY_H
#include "Singleton.h"
#include <map>
#include <vector>
#include <string>
template <typename Id, typename T, class C>
class Factory:public Singleton<C>
{
    using ID=Id;
    using Type=T;
    using Class=C;
    using CreatorFunc=Type*(*)(void);

public:
    Type* create(const ID & id);
    // Avoid cross-DLL delivery crashes caused by different CRT links
    // so name must be literal
    bool registerCreatorFunc(const ID &id, const char *name, const CreatorFunc &func);
    bool unregisterCreatorFunc(const ID & id);
    bool contain(const ID & id);
    const char * name(const ID & id);
    ID id(const char * name);

private:
    std::map<ID, const char*> m_nameMap;
    std::map<ID, CreatorFunc> m_creatorMap;
};

#endif // FACTORY_H
