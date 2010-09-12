/*
 * -----------------------------------------------------------------------------
 * Copyright (c) 2010, Matteo Bertozzi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL MATTEO BERTOZZI BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * -----------------------------------------------------------------------------
 */
#include <QBuffer>
#include <QHash>

#include "httprequest.h"

class HttpRequestPrivate {
    public:
        QHash<QByteArray, QByteArray> headers;
        HttpRequest::MethodType methodType;
        QByteArray methodName;
        QByteArray protocol;
        QByteArray content;
        QByteArray uri;
};

static QByteArray __knownHeader (HttpRequest::KnownHeaders header) {
    switch (header) {
        // http://tools.ietf.org/html/rfc4229
        case HttpRequest::AcceptHeader:            return("accept");
        case HttpRequest::AcceptCharsetHeader:     return("accept-charset");
        case HttpRequest::AcceptEncodingHeader:    return("accept-encoding");
        case HttpRequest::AcceptLanguageHeader:    return("accept-language");
        case HttpRequest::ContentEncodingHeader:   return("content-encoding");
        case HttpRequest::ContentLanguageHeader:   return("content-language");
        case HttpRequest::ContentLengthHeader:     return("content-length");
        case HttpRequest::ContentLocationHeader:   return("content-location");
        case HttpRequest::ContentTypeHeader:       return("content-type");
        case HttpRequest::HostHeader:              return("host");
        case HttpRequest::KeepAliveHeader:         return("keep-alive");
    }

    // NEVER REACHED!
    return(QByteArray());
}

HttpRequest::HttpRequest()
    : d(new HttpRequestPrivate)
{
    d->methodType = HttpRequest::MethodInvalid;
}

HttpRequest::HttpRequest (const HttpRequest& request)
    : d(new HttpRequestPrivate)
{
    d->headers = request.d->headers;
    d->methodType = request.d->methodType;
    d->methodName = request.d->methodName;
    d->protocol = request.d->protocol;
    d->content = request.d->content;
    d->uri = request.d->uri;
}

HttpRequest::~HttpRequest() {
    delete d;
}

bool HttpRequest::isNull (void) const {
    return(d->methodType == HttpRequest::MethodInvalid);
}

QByteArray HttpRequest::uri (void) const {
    return(d->uri);
}

void HttpRequest::setUri (const QByteArray& uri) {
    d->uri = uri;
}

QByteArray HttpRequest::protocol (void) const {
    return(d->protocol);
}

void HttpRequest::setProtocol (const QByteArray& protocol) {
    d->protocol = protocol;
}

HttpRequest::MethodType HttpRequest::method (void) const {
    return(d->methodType);
}

void HttpRequest::setMethod (HttpRequest::MethodType method) {
    switch (method) {
        case HttpRequest::MethodOptions:  d->methodName = "OPTIONS"; break;
        case HttpRequest::MethodGet:      d->methodName = "GET"; break;
        case HttpRequest::MethodHead:     d->methodName = "HEAD"; break;
        case HttpRequest::MethodPost:     d->methodName = "POST"; break;
        case HttpRequest::MethodPut:      d->methodName = "PUT"; break;
        case HttpRequest::MethodDelete:   d->methodName = "DELETE"; break;
        case HttpRequest::MethodTrace:    d->methodName = "TRACE"; break;
        case HttpRequest::MethodConnect:  d->methodName = "CONNECT"; break;
        default:
            break;
    }

    d->methodType = method;
}

const QByteArray HttpRequest::methodName (void) const {
    return(d->methodName);
}

void HttpRequest::setMethodName (const QByteArray& method) {
    d->methodName = method;
}

QByteArray HttpRequest::header (KnownHeaders header) const {
    return(d->headers.value(__knownHeader(header), QByteArray()));
}

QByteArray HttpRequest::header (const QByteArray& header) const {
    return(d->headers.value(header.toLower(), QByteArray()));
}

bool HttpRequest::containsHeader (KnownHeaders header) const {
    return(d->headers.contains(__knownHeader(header)));
}

bool HttpRequest::containsHeader (const QByteArray& header) const {
    return(d->headers.contains(header.toLower()));
}

QList<QByteArray> HttpRequest::headers (void) const {
    return(d->headers.keys());
}

void HttpRequest::setHeader (const QByteArray& key, const QByteArray& value) {
    d->headers.insert(key, value);
}

QByteArray HttpRequest::content (void) const {
    return(d->content);
}

void HttpRequest::setContent (const QByteArray& content) {
    d->content = content;
}


HttpRequest HttpRequest::fromData (const QByteArray& data) {
    QBuffer buffer;
    buffer.setData(data);
    buffer.open(QIODevice::ReadOnly);
    return(HttpRequest::fromStream(&buffer));
}

HttpRequest HttpRequest::fromStream (QIODevice *device) {
    HttpRequest request;
    QByteArray line;
    int index[2];

    if (!device->atEnd()) {
        line = device->readLine();

        if (line.startsWith("GET ")) {
            request.setMethod(HttpRequest::MethodGet);
            index[0] = 4;
        } else if (line.startsWith("POST ")) {
            request.setMethod(HttpRequest::MethodPost);
            index[0] = 5;
        } else if (line.startsWith("PUT ")) {
            request.setMethod(HttpRequest::MethodPut);
            index[0] = 4;
        } else if (line.startsWith("HEAD ")) {
            request.setMethod(HttpRequest::MethodHead);
            index[0] = 5;
        } else if (line.startsWith("DELETE ")) {
            request.setMethod(HttpRequest::MethodDelete);
            index[0] = 7;
        } else if (line.startsWith("OPTIONS ")) {
            request.setMethod(HttpRequest::MethodOptions);
            index[0] = 8;
        } else if (line.startsWith("TRACE ")) {
            request.setMethod(HttpRequest::MethodTrace);
            index[0] = 6;
        } else if (line.startsWith("CONNECT ")) {
            request.setMethod(HttpRequest::MethodConnect);
            index[0] = 8;
        } else {
            request.setMethod(HttpRequest::MethodExtension);
            index[0] = line.indexOf(' ') + 1;
        }

        index[1] = line.indexOf(' ', index[0]);
        request.setUri(line.mid(index[0], index[1] - index[0]));
        request.setProtocol(line.mid(index[1] + 1).trimmed());
    }

    while (!device->atEnd()) {
        line = device->readLine();
        if (line == "\r\n")
            break;

        index[0] = line.indexOf(':');
        QByteArray key = line.mid(0, index[0]).toLower();
        QByteArray value = line.mid(index[0] + 1).trimmed();
        request.setHeader(key, value);
    }

    int contentLength = 0;
    if (request.containsHeader(ContentLengthHeader))
        contentLength = request.header(ContentLengthHeader).toUInt();

    if (contentLength > 0) {
        QByteArray content;
        while (!device->atEnd() && contentLength > 0) {
            QByteArray data = device->read(contentLength);
            content.append(data.mid(0, qMin(data.length(), contentLength)));
            contentLength -= data.length();
        }

        request.setContent(content);
    }

    return(request);
}
//qDebug() << "Method:" << request.method() << request.methodName();
//qDebug() << "URI:" << request.uri();
//qDebug() << "PROTOCOL:" << request.protocol();
//qDebug() << "Headers:" << request.headers();
//qDebug() << "Content Length:" << request.header(HttpRequest::ContentLengthHeader).toUInt();
//qDebug() << "Connection:" << request.header("connection");
//qDebug() << "Content:" << request.content();
