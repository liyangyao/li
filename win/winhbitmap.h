/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2016/8/29

****************************************************************************/
#ifndef WINBITMAP_H
#define WINBITMAP_H

#include <Windows.h>
#include <QPixmap>

//need Qt += winextras

class WinHBITMAP
{
public:
    WinHBITMAP(int width, int height);
    ~WinHBITMAP();
    HDC hdc()
    {
        return m_hdc;
    }

    HBITMAP handle()
    {
        return m_handle;
    }

    int width()
    {
        return m_width;
    }

    int height()
    {
        return m_height;
    }

    QPixmap toQPixmap();


private:
    int m_width;
    int m_height;
    HDC m_hdc;
    HBITMAP m_handle;
    HGDIOBJ hbmOld;
};

#endif // WINBITMAP_H
