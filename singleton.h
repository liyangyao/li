/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/23

****************************************************************************/

#ifndef LI_SINGLETON_H
#define LI_SINGLETON_H

#include <memory>
#include <mutex>

namespace li{

template<typename T>
class Singleton
{
public:
    static T* get()
    {
        return m_value.get();
    }

    template<typename ...Args>
    static void create(Args&&... args)
    {
        std::call_once(m_onceFlag, [&]()
        {
            m_value.reset(new T(std::forward<Args>(args)...));
        });
    }

    Singleton(Singleton &) = delete;
    Singleton& operator=(Singleton &) = delete;

private:
    static std::once_flag m_onceFlag;
    static std::unique_ptr<T> m_value;
    Singleton() = default;
};

template<typename T>
class SingletonLazy
{
public:
    static T* get()
    {
        std::call_once(m_onceFlag, [&]()
        {
            m_value.reset(new T());
        });
        return m_value.get();
    }

    SingletonLazy(SingletonLazy &) = delete;
    SingletonLazy& operator=(SingletonLazy &) = delete;

private:
    static std::once_flag m_onceFlag;
    static std::unique_ptr<T> m_value;
    SingletonLazy() = default;
};

template<typename T>
std::once_flag Singleton<T>::m_onceFlag;

template<typename T>
std::unique_ptr<T> Singleton<T>::m_value;

template<typename T>
std::once_flag SingletonLazy<T>::m_onceFlag;

template<typename T>
std::unique_ptr<T> SingletonLazy<T>::m_value;

}

#endif // LI_SINGLETON_H
