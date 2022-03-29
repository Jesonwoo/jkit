#include "Singleton.h"
#include <cstdlib>
template<typename T>
static Singleton<T> m_instance;

template<typename T>
T &Singleton<T>::instance()
{
    return m_instance;
}
