/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/11/18

****************************************************************************/

#include "httpparserex.h"

http_parser_settings settings;

int HttpParserEx::on_message_begin(http_parser* parser)
{
    Q_ASSERT(posStart==-1);
    //qDebug()<<"on_message_begin";
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    self->posStart = parser->nread;
    self->data = std::make_shared<ParserData>(new ParserData);
    self->lastWasValue = false;
    return 0;
}

int HttpParserEx::on_url(http_parser* parser, const char *at, size_t length)
{
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    self->url.append(at, length);
    return 0;
}

int HttpParserEx::on_header_field(http_parser* parser, const char *at, size_t length)
{
    //qDebug()<<"on_header_field";
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    self->insertHeaderFieldValue();
    self->lastWasValue = false;
    self->headerValue.append(at, length);
    return 0;
}

int HttpParserEx::on_header_value(http_parser* parser, const char *at, size_t length)
{
    //qDebug()<<"on_header_value";
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    self->lastWasValue = true;
    self->headerValue.append(at, length);

    return 0;
}

int HttpParserEx::on_headers_complete(http_parser* parser)
{
    qDebug()<<"on_headers_complete content_length="<<parser->content_length;
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    self->insertHeaderFieldValue();
    self->data->url = QString::fromUtf8(self->url);
    self->data->method = parser->method;
    self->data->statusCode = parser->status_code;
    self->data->upgrade = parser->upgrade;
    self->data->shouldKeepAlive = http_should_keep_alive(parser);
    return 0;
}

int HttpParserEx::on_body(http_parser* parser, const char *at, size_t length)
{
    //qDebug()<<"on_body";
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    self->data->body.append(at, length);
    return 0;
}

int HttpParserEx::on_message_complete(http_parser* parser)
{
    qDebug()<<"on_message_complete content_length="<<parser->content_length;
    HttpParserEx* self  = reinterpret_cast<HttpParserEx*>(parser->data);
    int posEnd = parser->nread;
    self->data->raw.append(pCurrentData, posEnd - posStart);
    self->posStart = -1;
    self->m_dataList.append(self->data);
    self->data = nullptr;
    return 0;
}

void HttpParserEx::insertHeaderFieldValue()
{
    if (lastWasValue)
    {
        QString f = QString::fromUtf8(headerField).toLower();
        QString v = QString::fromUtf8(headerValue);
        data->hearders.insert(f, v);
        headerField.clear();
        headerValue.clear();
    }
}



HttpParserEx::HttpParserEx()
{
    posStart = -1;
}

bool HttpParserEx::execute(const char *data, int length)
{
    pCurrentData = data;
    int n = http_parser_execute(&parser, &settings, data, length);
    if (n<=length)
    {
        //error
        return;
    }
    foreach (const ParserDataPtr &d, m_dataList)
    {
        //callback
    }
    m_dataList.clear();
    if (posStart>=0)
    {
       data->raw.append(data, length - posStart);
       posStart = 0;//for next
    }
}

