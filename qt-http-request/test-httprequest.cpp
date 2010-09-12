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
#include <QCoreApplication>
#include <QDebug>

#include "httprequest.h"

static const char *data =
"GET /test HTTP/1.1\r\n"
"Connection: Keep-Alive\r\n"
"User-Agent: Mozilla/5.0 (compatible; Konqueror/3.2; Linux) (KHTML, like Gecko)\r\n"
"Accept: text/html, image/jpeg, image/png, text/*, image/*, */*\r\n"
"Accept-Encoding: x-gzip, x-deflate, gzip, deflate, identity\r\n"
"Accept-Charset: iso-8859-1, utf-8;q=0.5, *;q=0.5 \r\n"
"Accept-Language: de,en;q=0.7,en-us;q=0.3\r\n"
"Content-Length: 11\r\n"
"Host: test-host.org\r\n"
"\r\n"
"Hello World\r\n";


int main (int argc, char **argv) {
    QCoreApplication app(argc, argv);

    HttpRequest request = HttpRequest::fromData(data);
    qDebug() << "Method:" << request.method() << request.methodName();
    qDebug() << "URI:" << request.uri();
    qDebug() << "PROTOCOL:" << request.protocol();
    qDebug() << "Headers:" << request.headers();
    qDebug() << "Content Length:" << request.header(HttpRequest::ContentLengthHeader).toUInt();
    qDebug() << "Connection:" << request.header("connection");
    qDebug() << "Content:" << request.content();

    return(0);
}

