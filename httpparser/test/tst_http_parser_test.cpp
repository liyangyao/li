/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/30

****************************************************************************/

#include <QString>
#include <QtTest>
#include <httpparser/http_parser_pp.h>
#include <QTcpSocket>

class Http
{
private:
    QByteArray body;
    QMap<QString, QString> headers;
    QString path;
    QString url;
};

class Http_parser_test : public QObject
{
    Q_OBJECT

public:
    Http_parser_test();

private Q_SLOTS:    
    void No_ContentLength();
    void chunked();
    void Body_Test();
    void Upgrade_Test();
    void Network_Baidu_Test();

private:
    bool test(const QByteArray &d);
};

Http_parser_test::Http_parser_test()
{
    qDebug()<<http_parser_version();
    qDebug()<<"ULLONG_MAX="<<ULLONG_MAX;
}

bool Http_parser_test::test(const QByteArray &d)
{
    bool pass = false;

    HttpParser hp;
    QByteArray data;
    hp.onMessageBegin = [&]()
    {
        qDebug()<<"onMessageBegin";
        return 0;
    };

    hp.onBody = [&](const char *at, size_t length)->int
    {
        data.append(at, length);
        return 0;
    };

    hp.onMessageComplete = [&]()
    {
        qDebug()<<"body is ["<<data<<"]";
        qDebug()<<"onMessageComplete";
        pass = true;
        return 0;
    };
    hp.onChunkHeader = []()
    {
        qDebug()<<"onChunkHeader";
        return 0;
    };

    hp.onChunkComplete = []()
    {
        qDebug()<<"onChunkComplete";
        return 0;
    };

    hp.execute(d.constData(), d.length());
    return pass;
}

void Http_parser_test::No_ContentLength()
{
    bool pass = test("POST /no_contentlength HTTP/1.1\r\nConnection: close\r\n\r\nYes");

    //hp.execute(nullptr, 0);

    QVERIFY2(pass, "Failure");
}

void Http_parser_test::chunked()
{
    bool pass = test("POST /post HTTP/1.1\r\nTransfer-encoding: chunked\r\n\r\n"
                     "1\r\na\r\n"
                     "2\r\nab\r\n"
                     "0\r\n\r\n");
    QVERIFY2(pass, "Failure");
}

void Http_parser_test::Body_Test()
{
    bool pass = test("POST /post HTTP/1.1\r\nContent-Length: 3\r\n\r\nYes");
    QVERIFY2(pass, "Failure");
}

void Http_parser_test::Upgrade_Test()
{
    QByteArray input = "GET /demo HTTP/1.1\r\n"
            "Upgrade: WebSocket\r\n"
            "Connection: Upgrade\r\n"
            "Host: example.com\r\n"
            "Origin: http://example.com\r\n"
            "Content-Length: 5\r\n"
            "WebSocket-Protocol: sample\r\n\r\nWorld";
   HttpParser hp;
    QByteArray data;
    hp.onMessageBegin = [&]()
    {
        qDebug()<<"onMessageBegin";
        return 0;
    };

    hp.onBody = [&](const char *at, size_t length)->int
    {
        data.append(at, length);
        return 0;
    };

    hp.onMessageComplete = [&]()
    {
        qDebug()<<"body is ["<<data<<"]";
        qDebug()<<"onMessageComplete";
        return 0;
    };


    int n = hp.execute(input.constData(), input.length());
    if (hp.upgrade())
    {
        qDebug()<<"Is Upgrade";
    }
}

void Http_parser_test::Network_Baidu_Test()
{
    bool complete = false;
    QByteArray body;
    HttpParser hp(HTTP_BOTH);
    hp.onMessageBegin = [&]()
    {
        qDebug()<<"onMessageBegin";
        return 0;
    };

    hp.onBody = [&](const char *at, size_t length)->int
    {
        qDebug()<<"onBody";
        body.append(at, length);
        return 0;
    };

    hp.onMessageComplete = [&]()
    {
        qDebug()<<"body is ["<<body<<"]";
        qDebug()<<"onMessageComplete";
        complete = true;
        return 0;
    };


    QTcpSocket socket;
    socket.connectToHost("www.baidu.com", 80);
    if (!socket.waitForConnected())
    {
        QVERIFY2(false, "connect fail");
        return;
    }
    socket.write("GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    while (!complete)
    {
        if (!socket.waitForReadyRead(5*1000))
        {
            QVERIFY2(false, "socket error");
            return;
        }
        QByteArray data = socket.readAll();
        qDebug()<<"data:"<<data;
        hp.execute(data.constData(), data.size());
    }
}



QTEST_APPLESS_MAIN(Http_parser_test)

#include "tst_http_parser_test.moc"
