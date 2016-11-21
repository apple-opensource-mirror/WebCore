/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc.  All rights reserved.
 * Copyright (C) 2012 Samsung Electronics Ltd. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SocketStreamHandle_h
#define SocketStreamHandle_h

#include "SocketStreamHandleBase.h"

#if USE(SOUP)

#include "SessionID.h"
#include <wtf/RefCounted.h>
#include <wtf/glib/GRefPtr.h>

namespace WebCore {

class NetworkingContext;
class SocketStreamError;
class SocketStreamHandleClient;

class SocketStreamHandle final : public RefCounted<SocketStreamHandle>, public SocketStreamHandleBase {
public:
    static Ref<SocketStreamHandle> create(const URL& url, SocketStreamHandleClient& client, NetworkingContext&, SessionID) { return adoptRef(*new SocketStreamHandle(url, client)); }
    static Ref<SocketStreamHandle> create(GSocketConnection* socketConnection, SocketStreamHandleClient& client) { return adoptRef(*new SocketStreamHandle(socketConnection, client)); }

    virtual ~SocketStreamHandle();

private:
    SocketStreamHandle(const URL&, SocketStreamHandleClient&);
    SocketStreamHandle(GSocketConnection*, SocketStreamHandleClient&);

    int platformSend(const char* data, int length) override;
    void platformClose() override;

    void beginWaitingForSocketWritability();
    void stopWaitingForSocketWritability();

    static void connectedCallback(GSocketClient*, GAsyncResult*, SocketStreamHandle*);
    static void readReadyCallback(GInputStream*, GAsyncResult*, SocketStreamHandle*);
    static gboolean writeReadyCallback(GPollableOutputStream*, SocketStreamHandle*);

    void connected(GRefPtr<GSocketConnection>&&);
    void readBytes(gssize);
    void didFail(SocketStreamError&&);
    void writeReady();

    GRefPtr<GSocketConnection> m_socketConnection;
    GRefPtr<GInputStream> m_inputStream;
    GRefPtr<GPollableOutputStream> m_outputStream;
    GRefPtr<GSource> m_writeReadySource;
    GRefPtr<GCancellable> m_cancellable;
    std::unique_ptr<char[]> m_readBuffer;
};

}  // namespace WebCore

#endif

#endif  // SocketStreamHandle_h
