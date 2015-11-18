/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/11/18

****************************************************************************/

#ifndef HTTPPARSEREX_H
#define HTTPPARSEREX_H

#include <QMap>
#include <QHash>
#include <memory>
#include "httpparser/http_parser.h"

class ParserData
{
public:
    //respon
    unsigned int statusCode;
    //request
    unsigned int method;
    QString url;

    QHash<QString, QString> hearders;
    QByteArray body;

    //可选
    bool upgrade;
    bool shouldKeepAlive;

    //可选,整个请求或者回复数据
    QByteArray raw;
};
typedef std::shared_ptr<ParserData> ParserDataPtr;

class HttpParserEx
{
public:
    HttpParserEx();

private:
    static int on_message_begin(http_parser* parser);
    static int on_url(http_parser* parser, const char *at, size_t length);
    static int on_header_field(http_parser* parser, const char *at, size_t length);
    static int on_header_value(http_parser* parser, const char *at, size_t length);
    static int on_headers_complete(http_parser* parser);
    static int on_body(http_parser* parser, const char *at, size_t length);
    static int on_message_complete(http_parser* parser);

    QByteArray url;
    QByteArray headerField;
    QByteArray headerValue;
    bool lastWasValue;
    ParserDataPtr data;
    void insertHeaderFieldValue();
};

#endif // HTTPPARSEREX_H
