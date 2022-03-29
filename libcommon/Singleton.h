#ifndef SINGLETON_H
#define SINGLETON_H
/*
* 单实例模板
*/

// 不允许拷贝构造
class NonCopy
{
protected:
    NonCopy() = default;
    virtual ~NonCopy() = default;
    NonCopy(NonCopy&&) = delete;
    NonCopy& operator=(NonCopy&&) = delete;
    NonCopy(NonCopy const&) = delete;
    NonCopy& operator=(NonCopy const&) = delete;
};

template<typename T>
class Singleton : protected NonCopy
{
public:
    static T & instance(void);

private:
    static T m_instance;
};
#endif // SINGLETON_H
