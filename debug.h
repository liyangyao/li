/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/23

****************************************************************************/

#ifndef LI_DEBUG_H
#define LI_DEBUG_H

#include <memory>
#include <vector>
#include <thread>
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <li/singleton.h>
#include <li/blockingqueue.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace li {
class Debug
{
public:
    static void installMsgHandler(bool isOutputConsole = true, bool isOutputFile = false)
    {
        Singleton<Debug>::create(isOutputConsole, isOutputFile);
        if (isOutputFile)
        {
            std::thread t(&run, Singleton<Debug>::get());
            t.detach();
        }
        qInstallMessageHandler(ourMessageHandler);
    }

    static void installMsgHandlerByFile()
    {
        QString processName = getModuleName();
        QFileInfo f(processName);
        QString path = f.path();
        bool isOutputConsole = QFile::exists(path + "/" + "dbgview");
        bool isOutputFile = QFile::exists(path + "/" + "debug");
        installMsgHandler(isOutputConsole, isOutputFile);
    }

private:
    Debug(bool isOutputConsole, bool isOutputFile):
        m_isOutputConsole(isOutputConsole),
        m_isOutputFile(isOutputFile),
        m_lineCount(0)
    {

    }
    friend class Singleton<Debug>;
    struct Msg
    {
        SYSTEMTIME time;
        DWORD threadId;
        const char* level;
        QString msg;
    };
    typedef std::unique_ptr<Msg> MsgPtr;

    static void ourMessageHandler(QtMsgType type, const QMessageLogContext &/*context*/, const QString &s)
    {
        Debug* ins = Singleton<Debug>::get();
        if (!ins)
        {
            return;
        }
        if (ins->m_isOutputConsole)
        {
            OutputDebugString((LPCWSTR)s.utf16());
            OutputDebugString(L" --END\r\n");
        }
        if (ins->m_isOutputFile)
        {
            MsgPtr msg(new Msg);
            msg->msg = s;

            GetLocalTime(&msg->time);
            msg->threadId = GetCurrentThreadId();
            switch(type)
            {
            case QtDebugMsg:
                msg->level = "";
                break;
            case QtWarningMsg:
                msg->level = "[Warning]";
                break;
            case QtCriticalMsg:
                msg->level = "[Critical]";
                break;
            case QtFatalMsg:
                msg->level = "[Fatal]";
                break;
            }

            ins->m_msgQueue.put(std::move(msg));
        }
    }

    static QString getModuleName()
    {
        wchar_t buffer[MAX_PATH + 2];
        DWORD v = GetModuleFileName(0, buffer, MAX_PATH + 1);
        buffer[MAX_PATH + 1] = 0;

        if (v == 0)
            return QString();
        else
            return QString::fromWCharArray(buffer);
    }

    static QString getHostName()
    {
        wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 2];
        DWORD size = MAX_COMPUTERNAME_LENGTH;
        DWORD v = GetComputerName(buffer, &size);
        buffer[MAX_COMPUTERNAME_LENGTH + 1] = 0;

        if (v == 0)
            return QString();
        else
            return QString::fromWCharArray(buffer);
    }

    static const char *formatShortTime_c()
    {
        SYSTEMTIME lpsystime;
        GetLocalTime(&lpsystime);
        static char buff[32];
        _snprintf_s(buff, sizeof(buff), "%02u%02u%02u%02u%02u",lpsystime.wMonth,
                    lpsystime.wDay,lpsystime.wHour,lpsystime.wMinute,lpsystime.wSecond);
        return buff;
    }

    static QString formatShortTime()
    {
        return QString(formatShortTime_c());
    }



    static char* formatTime(const SYSTEMTIME& time)
    {
        static char time_buff[32];
        _snprintf_s(time_buff, sizeof(time_buff), "%u%02u%02u %02u:%02u:%02u.%03u ",
                    time.wYear,time.wMonth,
                    time.wDay,time.wHour,time.wMinute,
                    time.wSecond,time.wMilliseconds);
        return time_buff;
    }

    static QString generateFileName()
    {
        QString hostName = getHostName();
        QString processName = getModuleName();

        QFileInfo f(processName);
        QString baseName  =  f.completeBaseName();
        QString path = f.path();

        DWORD pid = GetCurrentProcessId();
        QString currentTime = formatShortTime();

        QString fileName;
        QTextStream(&fileName)<<path<<"/"<<baseName<<"."<<pid<<"."
                             <<currentTime<<"."<<hostName<<".log";
        return fileName;
    }

    void run()
    {
        OutputDebugStringA("EnterThread\r\n");
        std::unique_ptr<QFile> f;
        while (true)
        {
            MsgPtr msg = m_msgQueue.take();
            if (m_lineCount==0)
            {
                QString logFileName = generateFileName();
                f.reset(new QFile(logFileName));
                f->open(QFile::Append);
            }
            QTextStream stream(f.get());
            stream<<formatTime(msg->time)<<msg->threadId<<" "<<msg->level<<msg->msg<<"\r\n";
            m_lineCount++;
            const int MaxLineCount = 600*1000;
            if (m_lineCount>=MaxLineCount)
            {
                m_lineCount = 0;
            }
        }
    }

    BlockingQueue<MsgPtr> m_msgQueue;
    bool m_isOutputConsole;
    bool m_isOutputFile;
    int m_lineCount;
};
}

#endif // LI_UTILITY_H
