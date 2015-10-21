/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/9/23

****************************************************************************/

#ifndef LI_WRITEDUMP_H
#define LI_WRITEDUMP_H

#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

namespace li {

class WriteDump
{
public:
    static void install()
    {
        SetUnhandledExceptionFilter(myUnhandledExceptionFilter);
    }

private:
    static const wchar_t *formatShortTime_c()
    {
        SYSTEMTIME lpsystime;
        GetLocalTime(&lpsystime);
        static wchar_t buff[32];
        swprintf_s(buff, sizeof(buff), L"%02u%02u%02u%02u%02u", lpsystime.wMonth,
                    lpsystime.wDay,lpsystime.wHour,lpsystime.wMinute,lpsystime.wSecond);
        return buff;
    }

    static LONG WINAPI myUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
    {
        wchar_t szProgramPath[MAX_PATH + 2] = {0};
        if(GetModuleFileName(NULL, szProgramPath, MAX_PATH))
        {
            LPTSTR lpDot = wcsrchr(szProgramPath, '.');
            if(lpDot)
            {
                *lpDot = '\0';
            }
        }
        wchar_t szDumpFile[MAX_PATH] = {0};
        swprintf_s(szDumpFile, MAX_PATH, L"%s.%s.dmp", szProgramPath, formatShortTime_c());

        HANDLE lhDumpFile = CreateFile(szDumpFile, GENERIC_WRITE, 0, NULL,
                                       CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
        MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
        loExceptionInfo.ExceptionPointers = ExceptionInfo;
        loExceptionInfo.ThreadId = GetCurrentThreadId();
        loExceptionInfo.ClientPointers = true;

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile,
                          MiniDumpWithIndirectlyReferencedMemory, &loExceptionInfo,
                          NULL, NULL);
        CloseHandle(lhDumpFile);

        return EXCEPTION_EXECUTE_HANDLER;
    }
};
}

#endif // LI_WRITEDUMP_H
