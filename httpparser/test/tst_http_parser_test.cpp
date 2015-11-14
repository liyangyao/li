/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/30

****************************************************************************/

#include <QString>
#include <QtTest>
#include <httpparser/http_parser_pp.h>

class Http_parser_test : public QObject
{
    Q_OBJECT

public:
    Http_parser_test();

private Q_SLOTS:
    void No_ContentLength();
    void chunked();
    void Body_Test();

private:
    bool test(const QByteArray &d);
};

Http_parser_test::Http_parser_test()
{
    qDebug()<<http_parser_version();
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



QTEST_APPLESS_MAIN(Http_parser_test)

#include "tst_http_parser_test.moc"
