/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/10/21

****************************************************************************/

#ifndef TST_WMIQUERY
#define TST_WMIQUERY

#include <QString>
#include <QtTest>
#include <li/win/wmiquery.h>



class TstWmiQuery: public QObject
{
    Q_OBJECT

public:
    TstWmiQuery()
    {

    }
private:
    li::WmiQuery m_query;

private Q_SLOTS:
    void test()
    {
        li::WmiQuery query;
        query.exec("SELECT * FROM Win32_OperatingSystem");
        bool columnIsShow = false;



        while(query.next())
        {
            if (!columnIsShow)
            {
                columnIsShow = true;
                qDebug()<<query.columnList().join("  ");
            }
            QStringList list;
            for(int i=0; i<query.columnList().count(); i++)
            {
                list.append(query.value(i).toString());
            }
            qDebug()<<"Row:"<<list.join(",");
        }

        //query.columnList().append("Hello");
    }

    void getMacAddress()
    {
        m_query.exec("select MacAddress from  Win32_NetworkAdapter where PhysicalAdapter = true and NetEnabled = true");


        if(m_query.next())
        {
            qDebug()<<"MacAddress:"<< m_query.value(0).toString();
        }
        else{
            qDebug()<<m_query.lastError();
        }
    }

//    void getLoadPercentage()
//    {
//        m_query.exec("select LoadPercentage from  Win32_Processor");
//        if(m_query.next())
//        {
//            qDebug()<<"Cpu LoadPercentage:"<< m_query.value(0).toString();
//        }
//        else{
//            qDebug()<<m_query.lastError();
//        }
//    }

    void getOSCaption()
    {

        m_query.exec("select caption from  JWin32_OperatingSystem_JXXX");
        if(m_query.next())
        {
            qDebug()<<"Cpu LoadPercentage:"<< m_query.value(0).toString();
        }
        else{
            qDebug()<<"Cpu LoadPercentage error:"<<m_query.lastError();
        }
    }


};

#endif // TST_WMIQUERY

