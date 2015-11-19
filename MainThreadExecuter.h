/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/11/19

****************************************************************************/

#ifndef MAINTHREADEXECUTER
#define MAINTHREADEXECUTER

#include <QObject>
#include <functional>
#include <QThread>
#include <QCoreApplication>
#include <QMetaType>

class MainThreadExecuter : public QObject
{
    Q_OBJECT
public:
    typedef std::function<void()> MainThreadFunc;
    static void create()
    {
        instance();
    }

    static void send(const MainThreadFunc &func)
    {
        instance()->call(func, true);
    }

    static void post(const MainThreadFunc &func)
    {
        instance()->call(func, false);
    }

signals:
    void CallFunc(const MainThreadFunc &);
    void BlockCallFunc(const MainThreadFunc &);

private slots:
    void onFunc(const MainThreadFunc &func)
    {
        func();
    }

private:
    explicit MainThreadExecuter(QObject *parent = 0):
        QObject(parent)
    {
        qRegisterMetaType<MainThreadFunc>("MainThreadFunc");
        connect(this, SIGNAL(CallFunc(MainThreadFunc)), this, SLOT(onFunc(MainThreadFunc)), Qt::QueuedConnection);
        connect(this, SIGNAL(BlockCallFunc(MainThreadFunc)), this, SLOT(onFunc(MainThreadFunc)), Qt::BlockingQueuedConnection);
    }

    static MainThreadExecuter* instance()
    {
        static MainThreadExecuter ins;
        return &ins;
    }

    void call(const MainThreadFunc &func, bool isBlock)
    {
        if (QThread::currentThread()==qApp->thread())
        {
            //main thread
            func();
            return;
        }
        if (isBlock)
        {
            emit BlockCallFunc(func);
        }
        else{
            emit CallFunc(func);
        }
    }
};


#endif // MainThreadExecuter

