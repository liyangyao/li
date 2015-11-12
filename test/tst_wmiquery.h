/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/10/21

****************************************************************************/

#ifndef TST_WMIQUERY
#define TST_WMIQUERY

#include <QString>
#include <QtTest>
#include <li/win/wmiquery.h>
#include "rapidjson/jsonserialize.h"


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
//    void test()
//    {
//        li::WmiQuery query;
//        query.exec("SELECT * FROM Win32_OperatingSystem");
//        bool columnIsShow = false;



//        while(query.next())
//        {
//            if (!columnIsShow)
//            {
//                columnIsShow = true;
//                qDebug()<<query.columnList().join("  ");
//            }
//            QStringList list;
//            for(int i=0; i<query.columnList().count(); i++)
//            {
//                list.append(query.value(i).toString());
//            }
//            qDebug()<<"Row:"<<list.join(",");
//        }

//        //query.columnList().append("Hello");
//    }

//    void getMacAddress()
//    {
//        m_query.exec("select MacAddress from  Win32_NetworkAdapter where PhysicalAdapter = true and NetEnabled = true");


//        if(m_query.next())
//        {
//            qDebug()<<"MacAddress:"<< m_query.value(0).toString();
//        }
//        else{
//            qDebug()<<m_query.lastError();
//        }
//    }

//    void getCPUUsage()
//    {
//        m_query.exec("select LoadPercentage from Win32_Processor");
//        if(m_query.next())
//        {
//            qDebug()<<"Cpu LoadPercentage:"<< m_query.value(0).toString();
//        }
//        else{
//            qDebug()<<m_query.lastError();
//        }
//    }

//    void getOSCaption()
//    {
//        m_query.exec("select caption from  JWin32_OperatingSystem_JXXX");
//        if(m_query.next())
//        {
//            qDebug()<<"Cpu LoadPercentage:"<< m_query.value(0).toString();
//        }
//        else{
//            qDebug()<<"Cpu LoadPercentage error:"<<m_query.lastError();
//        }
//    }

    void toJson()
    {
        {
            Json::Serialize json;
            json<<"data";
            json.StartArray();
            for(int i=0; i<3; i++)
            {
                json.StartArray();
                json<<i<<i*10;
                json.EndArray();
            }
            json.EndArray();
            qDebug()<<json.toByteArray();
        }
        QString sql = "select MacAddress,PhysicalAdapter from  Win32_NetworkAdapter";
        li::WmiQuery query;
        query.exec(sql);
        Json::Serialize json;
        if (!query.next())
        {
            json<<"error"<<query.lastError();
        }
        else{
            //column
            json<<"column";
            json.StartArray();
            qDebug()<<"column:"<<query.columnList();
            foreach(const QString &caption, query.columnList())
            {
                json<<caption;
            }
            json.EndArray();
            json<<"data";
            json.StartArray();
            while(true)
            {
                json.StartArray();
                foreach(const QVariant &value, query.valueList())
                {
                    json<<value.toString();
                }
                json.EndArray();
                if (!query.next())
                {
                    break;
                }
            }
            json.EndArray();
        }
        qDebug()<< json.toByteArray();
    }


};

#endif // TST_WMIQUERY

