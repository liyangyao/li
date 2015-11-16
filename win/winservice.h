/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2014/11/6

install
uninstall
run

****************************************************************************/

#ifndef LI_WINSERVICE_H
#define LI_WINSERVICE_H

#include <windows.h>
#include <crtdbg.h>
#include <QObject>
#include <QFile>
#include <winevent.h>
#include <QCoreApplication>

#pragma comment(lib, "Advapi32.lib")

/*

int invoke_main(int argc, char *argv[])
{

}

int main(int argc, char *argv[])
{
    Args args(argc, argv);
    wchar_t *SERVICE_NAME = L"_Sample";
    wchar_t *SERVICE_DISPLAY = L"_Sample display";
    wchar_t *SERVICE_DESCRIPTION = L"_Sample description";

    QString arg1 = args[1];
    if (arg1=="-s")
    {
        li::WinService::start(SERVICE_NAME);
        return invoke_main(argc, argv);
    }
    else if (arg1=="-i")
    {
        li::WinService::Install(SERVICE_NAME, SERVICE_DISPLAY, SERVICE_AUTO_START, NULL, NULL, NULL, SERVICE_DESCRIPTION);
        return 0;
    }
    else if (arg1=="-u")
    {
        li::WinService::Uninstall(SERVICE_NAME);
        return 0;
    }
    else{
        return invoke_main(argc, argv);
    }
}

*/

namespace li
{

class WinService
{
public:
    static void start(PWSTR pszServiceName)
    {
         instance().run(pszServiceName);
    }

    static void start(const QString &serviceName)
    {
         instance().run((LPWSTR)serviceName.utf16());
    }

    static void install(PWSTR pszServiceName,
                        PWSTR pszDisplayName,
                        DWORD dwStartType,
                        PWSTR pszDependencies,
                        PWSTR pszAccount,
                        PWSTR pszPassword,
                        PWSTR pszDescription)
    {
        wchar_t szPath[MAX_PATH];
        if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
        {
            wprintf(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
            goto Cleanup;
        }
        wcscat_s(szPath, L" -s ");
        wcscat_s(szPath, pszServiceName);

        // Open the local default service control manager database
        SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT |
                                     SC_MANAGER_CREATE_SERVICE);
        if (hSCM == NULL)
        {
            wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
            goto Cleanup;
        }

        // Install the service into SCM by calling CreateService
        SC_HANDLE schService = CreateService(
                    hSCM,                   // SCManager database
                    pszServiceName,                 // Name of service
                    pszDisplayName,                 // Name to display
                    SERVICE_ALL_ACCESS,//SERVICE_QUERY_STATUS,           // Desired access
                    SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,      // Service type
                    dwStartType,                    // Service start type
                    SERVICE_ERROR_NORMAL,           // Error control type
                    szPath,                         // Service's binary
                    NULL,                           // No load ordering group
                    NULL,                           // No tag identifier
                    pszDependencies,                // Dependencies
                    pszAccount,                     // Service running account
                    pszPassword                     // Password of the account
                    );

        if (schService == NULL)
        {           
            goto Cleanup;
        }

        if (pszDescription)
        {
            SERVICE_DESCRIPTION desc;
            desc.lpDescription = pszDescription;
            ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &desc);
        }
        //启动服务
        SC_HANDLE hService = OpenService(hSCM, pszServiceName, SERVICE_START);
        if (hService)
        {
            QVector<const wchar_t *> argv(3);
            argv[0] = szPath;
            argv[1] = L" -s ";
            argv[2] = pszServiceName;

            StartService(hService, argv.size(), argv.data());
            CloseServiceHandle(hService);
        }
Cleanup:
        // Centralized cleanup for all allocated resources.
        if (hSCM)
        {
            CloseServiceHandle(hSCM);
            hSCM = NULL;
        }
        if (schService)
        {
            CloseServiceHandle(schService);
            schService = NULL;
        }
    }

    static void uninstall(PWSTR pszServiceName)
    {
        SC_HANDLE schSCManager = NULL;
        SC_HANDLE schService = NULL;
        SERVICE_STATUS ssSvcStatus = {};

        // Open the local default service control manager database
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
        if (schSCManager == NULL)
        {
            wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
            goto Cleanup;
        }

        // Open the service with delete, stop, and query status permissions
        schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP |
                                 SERVICE_QUERY_STATUS | DELETE);
        if (schService == NULL)
        {
            wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
            goto Cleanup;
        }

        // Try to stop the service
        if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
        {
            wprintf(L"Stopping %s.", pszServiceName);
            Sleep(1000);

            while (QueryServiceStatus(schService, &ssSvcStatus))
            {
                if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
                {
                    wprintf(L".");
                    Sleep(1000);
                }
                else break;
            }

            if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
            {
                wprintf(L"\n%s is stopped.\n", pszServiceName);
            }
            else
            {
                wprintf(L"\n%s failed to stop.\n", pszServiceName);
            }
        }

        // Now remove the service by calling DeleteService.
        if (!DeleteService(schService))
        {
            wprintf(L"DeleteService failed w/err 0x%08lx\n", GetLastError());
            goto Cleanup;
        }

        wprintf(L"%s is removed.\n", pszServiceName);

Cleanup:
        // Centralized cleanup for all allocated resources.
        if (schSCManager)
        {
            CloseServiceHandle(schSCManager);
            schSCManager = NULL;
        }
        if (schService)
        {
            CloseServiceHandle(schService);
            schService = NULL;
        }
    }

private:
    // The name of the service
    PWSTR m_name;

    // The status of the service
    SERVICE_STATUS m_status;

    // The service status handle
    SERVICE_STATUS_HANDLE m_statusHandle;
    WinEvent m_serviceEvent;

    void SetServiceStatus(DWORD dwCurrentState,
                                        DWORD dwWin32ExitCode = NO_ERROR,
                                        DWORD dwWaitHint = 0)
    {
        static DWORD dwCheckPoint = 1;

        // Fill in the SERVICE_STATUS structure of the service.

        m_status.dwCurrentState = dwCurrentState;
        m_status.dwWin32ExitCode = dwWin32ExitCode;
        m_status.dwWaitHint = dwWaitHint;

        m_status.dwCheckPoint =
            ((dwCurrentState == SERVICE_RUNNING) ||
            (dwCurrentState == SERVICE_STOPPED)) ?
            0 : dwCheckPoint++;

        // Report the status of the service to the SCM.
        ::SetServiceStatus(m_statusHandle, &m_status);
    }

    void Start()
    {

    }

    void Stop()
    {
        SetServiceStatus(SERVICE_STOPPED);
        qApp->quit();
    }

    void Pause()
    {
        SetServiceStatus(SERVICE_PAUSED);
    }

    void Continue()
    {
        SetServiceStatus(SERVICE_CONTINUE_PENDING);
    }

    void Shutdown()
    {
        SetServiceStatus(SERVICE_STOPPED);
        qApp->quit();
    }

    // Entry point for the service. It registers the handler function for the
    // service and starts the service.
    static void WINAPI ServiceMain(DWORD, LPWSTR *)
    {

        // Register the handler function for the service
        instance().m_statusHandle = RegisterServiceCtrlHandler(
                    instance().m_name, ServiceCtrlHandler);
        if (instance().m_statusHandle == NULL)
        {
            throw GetLastError();
        }

        instance().SetServiceStatus(SERVICE_RUNNING);
        instance().Start();
        instance().m_serviceEvent.notify();
    }

    static void WINAPI ServiceCtrlHandler(DWORD dwCtrl)
    {
        switch (dwCtrl)
        {
        case SERVICE_CONTROL_STOP: instance().Stop(); break;
        case SERVICE_CONTROL_PAUSE: instance().Pause(); break;
        case SERVICE_CONTROL_CONTINUE: instance().Continue(); break;
        case SERVICE_CONTROL_SHUTDOWN: instance().Shutdown(); break;
        case SERVICE_CONTROL_INTERROGATE: break;
        default: break;
        }
    }

    static void StartServiceCtrlDispatcher()
    {
        SERVICE_TABLE_ENTRY serviceTable[] =
        {
            { instance().m_name, ServiceMain },
            { NULL, NULL }
        };

        // Connects the main thread of a service process to the service control
        // manager, which causes the thread to be the service control dispatcher
        // thread for the calling process. This call returns when the service has
        // stopped. The process should simply terminate when the call returns.
        ::StartServiceCtrlDispatcher(serviceTable);
    }

    static WinService &instance()
    {
        static WinService ins;
        return ins;
    }

    void run(PWSTR pszServiceName)
    {
        // Service name must be a valid string and cannot be NULL.
        m_name = (pszServiceName == NULL) ? L"" : pszServiceName;

        m_statusHandle = NULL;

        // The service runs in its own process.
        m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

        // The service is starting.
        m_status.dwCurrentState = SERVICE_START_PENDING;

        // The accepted commands of the service.
        DWORD dwControlsAccepted = 0;
        dwControlsAccepted |= SERVICE_ACCEPT_STOP;
        dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
        dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
        m_status.dwControlsAccepted = dwControlsAccepted;

        m_status.dwWin32ExitCode = NO_ERROR;
        m_status.dwServiceSpecificExitCode = 0;
        m_status.dwCheckPoint = 0;
        m_status.dwWaitHint = 0;


        DWORD   dwThreadID;
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartServiceCtrlDispatcher, 0, 0, &dwThreadID);
        m_serviceEvent.waitForEvent();
    }
};

}

#endif // SERVICE_H
