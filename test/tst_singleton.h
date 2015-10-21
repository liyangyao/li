/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/10/21

****************************************************************************/

#ifndef SINGLETON_TST_H
#define SINGLETON_TST_H

#include <QString>
#include <QtTest>
#include <li/singleton.h>

namespace TstSingleton_inner{
class Object
{
public:
    int secret = 0;
};
}
using ObjectInstance = li::Singleton<TstSingleton_inner::Object>;
using ObjectLazyInstance = li::SingletonLazy<TstSingleton_inner::Object>;

class TstSingleton: public QObject
{
    Q_OBJECT

public:
    TstSingleton()
    {
        ObjectInstance::create();
    }

private Q_SLOTS:
    void test()
    {
        QVERIFY(ObjectInstance::get()->secret == 0);
        ObjectInstance::get()->secret = 100;
        QVERIFY(ObjectInstance::get()->secret == 100);
    }

    void testLazy()
    {
        QVERIFY(ObjectLazyInstance::get()->secret == 0);
        ObjectLazyInstance::get()->secret = 101;
        QVERIFY(ObjectLazyInstance::get()->secret == 101);


    }
};



#endif // SINGLETON_TST_H

