/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2014/11/7

****************************************************************************/

#ifndef LI_WINDOWSEVENT_H
#define LI_WINDOWSEVENT_H

#include <windows.h>

namespace li
{
class WinEvent
{
public:
    explicit WinEvent(const LPWSTR name = NULL)
    {
        m_hEvent = CreateEvent(0, FALSE, FALSE, name);
    }

    virtual ~WinEvent()
    {
        if (m_hEvent)
        {
            CloseHandle(m_hEvent);
        }
    }

    void notify()
    {
        if (m_hEvent)
        {
            SetEvent(m_hEvent);
        }
    }

    void waitForEvent(DWORD milliseconds = INFINITE)
    {
        if (m_hEvent)
        {
            WaitForSingleObject(m_hEvent, milliseconds);
        }
    }

    HANDLE getHandle() const { return m_hEvent; }

protected:
    HANDLE m_hEvent;
};
}

#endif // WINDOWSEVENT_H
