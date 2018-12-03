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
#include "JSTestNamedDeleterWithIndexedGetter.h"

#include "JSDOMAbstractOperations.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMWrapperCache.h"
#include "URL.h"
#include <JavaScriptCore/FunctionPrototype.h>
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/PropertyNameArray.h>
#include <wtf/GetPtr.h>
#include <wtf/PointerPreparations.h>


namespace WebCore {
using namespace JSC;

// Attributes

JSC::EncodedJSValue jsTestNamedDeleterWithIndexedGetterConstructor(JSC::ExecState*, JSC::EncodedJSValue, JSC::PropertyName);
bool setJSTestNamedDeleterWithIndexedGetterConstructor(JSC::ExecState*, JSC::EncodedJSValue, JSC::EncodedJSValue);

class JSTestNamedDeleterWithIndexedGetterPrototype : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestNamedDeleterWithIndexedGetterPrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestNamedDeleterWithIndexedGetterPrototype* ptr = new (NotNull, JSC::allocateCell<JSTestNamedDeleterWithIndexedGetterPrototype>(vm.heap)) JSTestNamedDeleterWithIndexedGetterPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestNamedDeleterWithIndexedGetterPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};

using JSTestNamedDeleterWithIndexedGetterConstructor = JSDOMConstructorNotConstructable<JSTestNamedDeleterWithIndexedGetter>;

template<> JSValue JSTestNamedDeleterWithIndexedGetterConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestNamedDeleterWithIndexedGetterConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->prototype, JSTestNamedDeleterWithIndexedGetter::prototype(vm, globalObject), JSC::PropertyAttribute::DontDelete | JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(&vm, String("TestNamedDeleterWithIndexedGetter"_s)), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
}

template<> const ClassInfo JSTestNamedDeleterWithIndexedGetterConstructor::s_info = { "TestNamedDeleterWithIndexedGetter", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestNamedDeleterWithIndexedGetterConstructor) };

/* Hash table for prototype */

static const HashTableValue JSTestNamedDeleterWithIndexedGetterPrototypeTableValues[] =
{
    { "constructor", static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestNamedDeleterWithIndexedGetterConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTestNamedDeleterWithIndexedGetterConstructor) } },
};

const ClassInfo JSTestNamedDeleterWithIndexedGetterPrototype::s_info = { "TestNamedDeleterWithIndexedGetterPrototype", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestNamedDeleterWithIndexedGetterPrototype) };

void JSTestNamedDeleterWithIndexedGetterPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestNamedDeleterWithIndexedGetter::info(), JSTestNamedDeleterWithIndexedGetterPrototypeTableValues, *this);
}

const ClassInfo JSTestNamedDeleterWithIndexedGetter::s_info = { "TestNamedDeleterWithIndexedGetter", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestNamedDeleterWithIndexedGetter) };

JSTestNamedDeleterWithIndexedGetter::JSTestNamedDeleterWithIndexedGetter(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestNamedDeleterWithIndexedGetter>&& impl)
    : JSDOMWrapper<TestNamedDeleterWithIndexedGetter>(structure, globalObject, WTFMove(impl))
{
}

void JSTestNamedDeleterWithIndexedGetter::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(vm, info()));

}

JSObject* JSTestNamedDeleterWithIndexedGetter::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestNamedDeleterWithIndexedGetterPrototype::create(vm, &globalObject, JSTestNamedDeleterWithIndexedGetterPrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestNamedDeleterWithIndexedGetter::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestNamedDeleterWithIndexedGetter>(vm, globalObject);
}

JSValue JSTestNamedDeleterWithIndexedGetter::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestNamedDeleterWithIndexedGetterConstructor>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestNamedDeleterWithIndexedGetter::destroy(JSC::JSCell* cell)
{
    JSTestNamedDeleterWithIndexedGetter* thisObject = static_cast<JSTestNamedDeleterWithIndexedGetter*>(cell);
    thisObject->JSTestNamedDeleterWithIndexedGetter::~JSTestNamedDeleterWithIndexedGetter();
}

bool JSTestNamedDeleterWithIndexedGetter::getOwnPropertySlot(JSObject* object, ExecState* state, PropertyName propertyName, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSTestNamedDeleterWithIndexedGetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (auto index = parseIndex(propertyName)) {
        if (index.value() < thisObject->wrapped().length()) {
            auto value = toJS<IDLDOMString>(*state, thisObject->wrapped().item(index.value()));
            slot.setValue(thisObject, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly), value);
            return true;
        }
        return JSObject::getOwnPropertySlot(object, state, propertyName, slot);
    }
    using GetterIDLType = IDLDOMString;
    auto getterFunctor = [] (auto& thisObject, auto propertyName) -> std::optional<typename GetterIDLType::ImplementationType> {
        auto result = thisObject.wrapped().namedItem(propertyNameToAtomicString(propertyName));
        if (!GetterIDLType::isNullValue(result))
            return typename GetterIDLType::ImplementationType { GetterIDLType::extractValueFromNullable(result) };
        return std::nullopt;
    };
    if (auto namedProperty = accessVisibleNamedProperty<OverrideBuiltins::No>(*state, *thisObject, propertyName, getterFunctor)) {
        auto value = toJS<IDLDOMString>(*state, WTFMove(namedProperty.value()));
        slot.setValue(thisObject, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly), value);
        return true;
    }
    return JSObject::getOwnPropertySlot(object, state, propertyName, slot);
}

bool JSTestNamedDeleterWithIndexedGetter::getOwnPropertySlotByIndex(JSObject* object, ExecState* state, unsigned index, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSTestNamedDeleterWithIndexedGetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (LIKELY(index <= MAX_ARRAY_INDEX)) {
        if (index < thisObject->wrapped().length()) {
            auto value = toJS<IDLDOMString>(*state, thisObject->wrapped().item(index));
            slot.setValue(thisObject, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly), value);
            return true;
        }
        return JSObject::getOwnPropertySlotByIndex(object, state, index, slot);
    }
    auto propertyName = Identifier::from(state, index);
    using GetterIDLType = IDLDOMString;
    auto getterFunctor = [] (auto& thisObject, auto propertyName) -> std::optional<typename GetterIDLType::ImplementationType> {
        auto result = thisObject.wrapped().namedItem(propertyNameToAtomicString(propertyName));
        if (!GetterIDLType::isNullValue(result))
            return typename GetterIDLType::ImplementationType { GetterIDLType::extractValueFromNullable(result) };
        return std::nullopt;
    };
    if (auto namedProperty = accessVisibleNamedProperty<OverrideBuiltins::No>(*state, *thisObject, propertyName, getterFunctor)) {
        auto value = toJS<IDLDOMString>(*state, WTFMove(namedProperty.value()));
        slot.setValue(thisObject, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly), value);
        return true;
    }
    return JSObject::getOwnPropertySlotByIndex(object, state, index, slot);
}

void JSTestNamedDeleterWithIndexedGetter::getOwnPropertyNames(JSObject* object, ExecState* state, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    auto* thisObject = jsCast<JSTestNamedDeleterWithIndexedGetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(object, info());
    for (unsigned i = 0, count = thisObject->wrapped().length(); i < count; ++i)
        propertyNames.add(Identifier::from(state, i));
    for (auto& propertyName : thisObject->wrapped().supportedPropertyNames())
        propertyNames.add(Identifier::fromString(state, propertyName));
    JSObject::getOwnPropertyNames(object, state, propertyNames, mode);
}

bool JSTestNamedDeleterWithIndexedGetter::deleteProperty(JSCell* cell, ExecState* state, PropertyName propertyName)
{
    auto& thisObject = *jsCast<JSTestNamedDeleterWithIndexedGetter*>(cell);
    auto& impl = thisObject.wrapped();
    if (auto index = parseIndex(propertyName))
        return !impl.isSupportedPropertyIndex(index.value());
    if (isVisibleNamedProperty<OverrideBuiltins::No>(*state, thisObject, propertyName)) {
        return impl.deleteNamedProperty(propertyNameToString(propertyName));
    }
    return JSObject::deleteProperty(cell, state, propertyName);
}

bool JSTestNamedDeleterWithIndexedGetter::deletePropertyByIndex(JSCell* cell, ExecState* state, unsigned index)
{
    auto& thisObject = *jsCast<JSTestNamedDeleterWithIndexedGetter*>(cell);
    auto& impl = thisObject.wrapped();
    return !impl.isSupportedPropertyIndex(index);
}

EncodedJSValue jsTestNamedDeleterWithIndexedGetterConstructor(ExecState* state, EncodedJSValue thisValue, PropertyName)
{
    VM& vm = state->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestNamedDeleterWithIndexedGetterPrototype*>(vm, JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(state, throwScope);
    return JSValue::encode(JSTestNamedDeleterWithIndexedGetter::getConstructor(state->vm(), prototype->globalObject()));
}

bool setJSTestNamedDeleterWithIndexedGetterConstructor(ExecState* state, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    VM& vm = state->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestNamedDeleterWithIndexedGetterPrototype*>(vm, JSValue::decode(thisValue));
    if (UNLIKELY(!prototype)) {
        throwVMTypeError(state, throwScope);
        return false;
    }
    // Shadowing a built-in constructor
    return prototype->putDirect(vm, vm.propertyNames->constructor, JSValue::decode(encodedValue));
}

bool JSTestNamedDeleterWithIndexedGetterOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSTestNamedDeleterWithIndexedGetterOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestNamedDeleterWithIndexedGetter = static_cast<JSTestNamedDeleterWithIndexedGetter*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestNamedDeleterWithIndexedGetter->wrapped(), jsTestNamedDeleterWithIndexedGetter);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestNamedDeleterWithIndexedGetter@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore33TestNamedDeleterWithIndexedGetterE[]; }
#endif
#endif

JSC::JSValue toJSNewlyCreated(JSC::ExecState*, JSDOMGlobalObject* globalObject, Ref<TestNamedDeleterWithIndexedGetter>&& impl)
{

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl.ptr()));
#if PLATFORM(WIN)
    void* expectedVTablePointer = WTF_PREPARE_VTBL_POINTER_FOR_INSPECTION(__identifier("??_7TestNamedDeleterWithIndexedGetter@WebCore@@6B@"));
#else
    void* expectedVTablePointer = WTF_PREPARE_VTBL_POINTER_FOR_INSPECTION(&_ZTVN7WebCore33TestNamedDeleterWithIndexedGetterE[2]);
#endif

    // If this fails TestNamedDeleterWithIndexedGetter does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    static_assert(std::is_polymorphic<TestNamedDeleterWithIndexedGetter>::value, "TestNamedDeleterWithIndexedGetter is not polymorphic");

    // If you hit this assertion you either have a use after free bug, or
    // TestNamedDeleterWithIndexedGetter has subclasses. If TestNamedDeleterWithIndexedGetter has subclasses that get passed
    // to toJS() we currently require TestNamedDeleterWithIndexedGetter you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createWrapper<TestNamedDeleterWithIndexedGetter>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::ExecState* state, JSDOMGlobalObject* globalObject, TestNamedDeleterWithIndexedGetter& impl)
{
    return wrap(state, globalObject, impl);
}

TestNamedDeleterWithIndexedGetter* JSTestNamedDeleterWithIndexedGetter::toWrapped(JSC::VM& vm, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestNamedDeleterWithIndexedGetter*>(vm, value))
        return &wrapper->wrapped();
    return nullptr;
}

}
