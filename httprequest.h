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

