/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(DATABASE)

#include "JSTestCallback.h"

#include "JSClass1.h"
#include "JSClass2.h"
#include "ScriptExecutionContext.h"
#include <runtime/JSLock.h>
#include <wtf/MainThread.h>

using namespace JSC;

namespace WebCore {

JSTestCallback::JSTestCallback(JSObject* callback, JSDOMGlobalObject* globalObject)
    : m_data(new JSCallbackData(callback, globalObject))
    , m_isolatedWorld(globalObject->world())
{
}

JSTestCallback::~JSTestCallback()
{
    callOnMainThread(JSCallbackData::deleteData, m_data);
#ifndef NDEBUG
    m_data = 0;
#endif
}

// Functions

bool JSTestCallback::callbackWithClass1Param(ScriptExecutionContext* context, Class1* class1Param)
{
    ASSERT(m_data);
    ASSERT(context);

    RefPtr<JSTestCallback> protect(this);

    JSLock lock(SilenceAssertionsOnly);

    JSDOMGlobalObject* globalObject = toJSDOMGlobalObject(context, m_isolatedWorld.get());
    if (!globalObject)
        return true;

    ExecState* exec = globalObject->globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS(exec, class1Param));

    bool raisedException = false;
    m_data->invokeCallback(args, &raisedException);
    return !raisedException;
}

bool JSTestCallback::callbackWithClass2Param(ScriptExecutionContext* context, Class2* class2Param, const String& strArg)
{
    ASSERT(m_data);
    ASSERT(context);

    RefPtr<JSTestCallback> protect(this);

    JSLock lock(SilenceAssertionsOnly);

    JSDOMGlobalObject* globalObject = toJSDOMGlobalObject(context, m_isolatedWorld.get());
    if (!globalObject)
        return true;

    ExecState* exec = globalObject->globalExec();
    MarkedArgumentBuffer args;
    args.append(toJS(exec, class2Param));
    args.append(toJS(exec, strArg));

    bool raisedException = false;
    m_data->invokeCallback(args, &raisedException);
    return !raisedException;
}

}

#endif // ENABLE(DATABASE)
