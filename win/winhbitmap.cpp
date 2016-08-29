/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2016/8/29

****************************************************************************/
#include "winhbitmap.h"
#include <QtWinExtras/QtWin>

WinHBITMAP::WinHBITMAP(int width, int height):
    m_width(width),
    m_height(height)
{
    hdcScreen = GetDC(NULL);
    m_hdc = CreateCompatibleDC(hdcScreen);
    m_handle = CreateCompatibleBitmap(hdcScreen, width, height);
    hbmOld = SelectObject(m_hdc, m_handle);
}

WinHBITMAP::~WinBitmap()
{
    SelectObject(m_hdc, hbmOld);
    DeleteObject(m_handle);
    DeleteDC(m_hdc);
    ReleaseDC(NULL, hdcScreen);
}

QPixmap WinHBITMAP::toQPixmap()
{
    return QPixmap(QtWin::fromHBITMAP(m_handle));
}
