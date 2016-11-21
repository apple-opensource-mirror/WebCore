/*
 * Copyright (C) 2016 Canon Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions
 * are required to be met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Canon Inc. nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CANON INC. AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CANON INC. AND ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "JSFetchResponse.h"

#if ENABLE(FETCH_API)

#include "JSReadableStreamSource.h"

namespace WebCore {

JSC::JSValue JSFetchResponse::body(JSC::ExecState& state) const
{
#if ENABLE(STREAMS_API)
    if (!m_body) {
        JSC::JSValue readableStream;
        if (wrapped().isDisturbed()) {
            readableStream = createReadableStream(state, globalObject(), nullptr);
            // Let's get the reader to lock it.
            getReadableStreamReader(state, readableStream);
        } else {
            ReadableStreamSource* source = wrapped().createReadableStreamSource();
            readableStream = source ? createReadableStream(state, globalObject(), source) : JSC::jsNull();
        }
        m_body.set(state.vm(), this, readableStream);
    }
    return m_body.get();
#else
    UNUSED_PARAM(state);
    return JSC::jsNull();
#endif
}

} // namespace WebCore

#endif // ENABLE(FETCH_API)
