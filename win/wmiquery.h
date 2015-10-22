/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/10/21

****************************************************************************/

#ifndef LI_WMIQUERY
#define LI_WMIQUERY

#include <QUuid>
#include <WbemCli.h>
#include <comdef.h>
#include <qaxtypes.h>
#include <QVariant>
#include <QStringList>

#pragma comment(lib, "wbemuuid.lib")

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

namespace li{
class WmiQuery
{
public:
    WmiQuery()
    {
        pLoc = NULL;
        pSvc = NULL;
        pEnumerator = NULL;
        m_hasResult = false;
        if (!init())
        {
            release();
        }
    }

    void exec(const QString &sql)
    {
        if (m_hasResult)
        {
            m_hasResult = false;
            m_columnList.clear();
            m_valueList.clear();
        }
        if (!pLoc || !pSvc)
        {
            return;
        }
        if (pEnumerator)
        {
            pEnumerator->Release();
            pEnumerator = NULL;
        }
        m_lastRes = pSvc->ExecQuery(
                    bstr_t("WQL"),
                    (BSTR)sql.utf16(),
                    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                    NULL,
                    &pEnumerator);

        if (FAILED(m_lastRes))
        {
            m_error = QLatin1String("Unable to launch pSvc->ExecQuery");
            return;
        }
    }

    bool next()
    {
        if (!pEnumerator) return false;
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;
        m_lastRes = pEnumerator->Next(WBEM_INFINITE, 1,
                                       &pclsObj, &uReturn);

        if (FAILED(m_lastRes))
        {
            m_error = QLatin1String("Unable to pEnumerator->Next");
            return false;
        }


        if(0 == uReturn)
        {
            return false;
        }

        m_lastRes = pclsObj->BeginEnumeration(WBEM_FLAG_LOCAL_ONLY);
        do {
            BSTR bstrName;
            VARIANT Value;
            m_lastRes = pclsObj->Next(0, &bstrName, &Value, NULL, NULL);
            if (WBEM_S_NO_ERROR == m_lastRes)
            {
                QVariant v = VARIANTToQVariant(Value, NULL);
                //qDebug()<<"bstrName:"<<QString::fromWCharArray(bstrName)<<" value:"<<v.toString();
                m_valueList.append(v);
                if (!m_hasResult)
                {
                    m_columnList.append(QString::fromWCharArray(bstrName));
                }
            }
        }while ( WBEM_S_NO_ERROR == m_lastRes );
        m_hasResult = true;
        m_lastRes = pclsObj->EndEnumeration();
        pclsObj->Release();
        return true;
    }

    QVariant value(int col)
    {
        return m_valueList[col];
    }

    const QStringList& columnList()
    {
        return m_columnList;
    }

    QString lastError()
    {
        return m_error + ":0x" + QString::number(m_lastRes, 16);
    }

    ~WmiQuery()
    {
        release();
    }

private:
    IWbemLocator *pLoc;
    IWbemServices *pSvc;
    IEnumWbemClassObject* pEnumerator;
    QStringList m_columnList;
    QVariantList m_valueList;
    QString m_error;
    HRESULT m_lastRes;
    bool m_hasResult;

    bool init()
    {
        m_lastRes = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(m_lastRes))
        {
            m_error = QLatin1String("Unable to launch COM");
            return false;
        }

        m_lastRes = CoInitializeSecurity(NULL, -1, NULL, NULL,
                                         RPC_C_AUTHN_LEVEL_DEFAULT,
                                         RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
                                         EOAC_NONE, NULL);
        if (FAILED(m_lastRes))
        {
            m_error = QLatin1String("Unable to CoInitializeSecurity");
            return false;
        }

        m_lastRes = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                    IID_IWbemLocator, (LPVOID *) &pLoc);
        if (FAILED(m_lastRes))
        {
            m_error = QLatin1String("Unable to create a WbemLocator");
            CoUninitialize();
            return false;
        }

        m_lastRes = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0,
                    NULL, 0, 0, &pSvc);

        if (FAILED(m_lastRes))
        {            
            m_error = QLatin1String("Unable to connect to \"CIMV2\"");
            pLoc->Release();
            CoUninitialize();
            return false;
        }

//        m_lastRes = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
//                    NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
//                    NULL, EOAC_NONE);

//        if (FAILED(m_lastRes))
//        {
//            m_error = QLatin1String("Unable to CoSetProxyBlanket");
//            return false;
//        }
        return true;
    }

    void release()
    {
        if (pSvc)
        {
            pSvc->Release();
            pSvc = NULL;
        }
        if (pLoc)
        {
            pLoc->Release();
            pLoc = NULL;
        }

        if (pEnumerator)
        {
            pEnumerator->Release();
            pEnumerator = NULL;
        }
    }

    QString winError(DWORD err)
    {
        LPWSTR bufPtr = NULL;

        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, err, 0, (LPWSTR)&bufPtr, 0, NULL);
        const QString result =
                (bufPtr) ? QString::fromUtf16((const ushort*)bufPtr).trimmed() :
                           QString("Unknown Error %1").arg(err);
        LocalFree(bufPtr);
        return result;
    }
};
}
#endif // WMIQUERY

