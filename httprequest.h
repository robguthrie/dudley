#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <QByteArray>

class HttpRequestPrivate;
class HttpRequest {
    public:
        enum MethodType {
            MethodInvalid,
            MethodOptions,
            MethodGet,
            MethodHead,
            MethodPost,
            MethodPut,
            MethodDelete,
            MethodTrace,
            MethodConnect,
            MethodExtension,
        };

        enum KnownHeaders {
            AcceptHeader,
            AcceptCharsetHeader,
            AcceptEncodingHeader,
            AcceptLanguageHeader,
            ContentEncodingHeader,
            ContentLanguageHeader,
            ContentLengthHeader,
            ContentLocationHeader,
            ContentTypeHeader,
            HostHeader,
            KeepAliveHeader,
        };

    public:
        HttpRequest();
        HttpRequest (const HttpRequest& request);
        ~HttpRequest();

        bool isNull (void) const;

        QByteArray uri (void) const;
        void setUri (const QByteArray& uri);

        QByteArray protocol (void) const;
        void setProtocol (const QByteArray& protocol);

        MethodType method (void) const;
        void setMethod (MethodType method);

        const QByteArray methodName (void) const;
        void setMethodName (const QByteArray& method);

        QByteArray header (KnownHeaders header) const;
        QByteArray header (const QByteArray& header) const;
        bool containsHeader (KnownHeaders header) const;
        bool containsHeader (const QByteArray& header) const;
        QList<QByteArray> headers (void) const;
        void setHeader (const QByteArray& key, const QByteArray& value);

        QByteArray content (void) const;
        void setContent (const QByteArray& content);

    public:
        static HttpRequest fromData (const QByteArray& data);
        static HttpRequest fromStream (QIODevice *buffer);

    private:
        HttpRequestPrivate *d;
};

#endif /* !_HTTP_REQUEST_ */

