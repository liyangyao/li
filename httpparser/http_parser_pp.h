/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/25

****************************************************************************/

#ifndef UVPP_HTTPPARSER_H
#define UVPP_HTTPPARSER_H

#include <functional>
#include <memory>
#include <httpparser/http_parser.h>

class HttpParser
{
public:
    typedef std::function<int()> HttpCallback;
    typedef std::function<int(const char *at, size_t length)> HttpDataCallback;
    explicit HttpParser()
    {
        m_parser.data = this;
    }

    size_t execute(const char *data, size_t len)
    {
        if (!m_settings)
        {
            m_settings.reset(new http_parser_settings);
            m_settings->on_message_begin = on_message_begin;
            m_settings->on_url = on_url;
            m_settings->on_header_field = on_header_field;
            m_settings->on_header_value = on_header_value;
            m_settings->on_headers_complete = on_headers_complete;
            m_settings->on_body = on_body;
            m_settings->on_message_complete = on_message_complete;
            m_settings->on_chunk_header = on_chunk_header;
            m_settings->on_chunk_complete = on_chunk_complete;
            http_parser_init(&m_parser, HTTP_REQUEST);

        }
        int ret = http_parser_execute(&m_parser, m_settings.get(), data, len);
        return ret;
    }

    unsigned short statusCode()
    {
        return m_parser.status_code;
    }

    const char * method()
    {
        return http_method_str((http_method)m_parser.method);
    }

    bool upgrade()
    {
        return m_parser.upgrade;
    }

    const char * httpError()
    {
        return http_errno_description((http_errno)m_parser.http_errno);
    }

    bool shouldKeepAlive()
    {
        return http_should_keep_alive(&m_parser);
    }

    uint32_t nread()
    {
        return m_parser.nread;
    }

    HttpCallback onMessageBegin;
    HttpDataCallback onUrl;
    HttpDataCallback onHeaderField;
    HttpDataCallback onHeaderValue;
    HttpCallback onHeadersComplete;
    HttpDataCallback onBody;
    HttpCallback onMessageComplete;
    HttpCallback onChunkHeader;
    HttpCallback onChunkComplete;

private:
    http_parser m_parser;
    std::unique_ptr<http_parser_settings> m_settings;
    static int on_message_begin(http_parser* parser)
    {
        //qDebug()<<"on_message_begin";
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onMessageBegin)
        {
            return self->onMessageBegin();
        }
        return 0;
    }

    static int on_url(http_parser* parser, const char *at, size_t length)
    {
        qDebug()<<"on_url nread="<<parser->nread;
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onUrl)
        {
            return self->onUrl(at, length);
        }
        return 0;
    }

    static int on_header_field(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_header_field";
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onHeaderField)
        {
            return self->onHeaderField(at, length);
        }
        return 0;
    }

    static int on_header_value(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_header_value";
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onHeaderValue)
        {
            return self->onHeaderValue(at, length);
        }
        return 0;
    }

    static int on_headers_complete(http_parser* parser)
    {
        qDebug()<<"on_headers_complete content_length="<<parser->content_length;
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onHeadersComplete)
        {
            return self->onHeadersComplete();
        }
        return 0;
    }

    static int on_body(http_parser* parser, const char *at, size_t length)
    {
        //qDebug()<<"on_body";
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onBody)
        {
            return self->onBody(at, length);
        }
        return 0;
    }

    static int on_message_complete(http_parser* parser)
    {
        qDebug()<<"on_message_complete content_length="<<parser->content_length;
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onMessageComplete)
        {
            return self->onMessageComplete();
        }
        return 0;
    }

    static int on_chunk_header(http_parser* parser)
    {
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onChunkHeader)
        {
            return self->onChunkHeader();
        }
        return 0;
    }

    static int on_chunk_complete(http_parser* parser)
    {
        HttpParser* self  = reinterpret_cast<HttpParser*>(parser->data);
        if (self->onChunkComplete)
        {
            return self->onChunkComplete();
        }
        return 0;
    }
};

#endif // HTTPPARSER_H
