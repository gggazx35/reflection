#pragma once

#include "TypeResolver.h"
#include "GarbageCollector.h"
#define __REFLECTION_START(type) \
void* type::operator new(size_t size) { \
	void* obj = reinterpret_cast<type*>(GarbageCollector::get()->Allocate(size, &reflect)); \
	return obj; \
} \
void type::initTypeDescriptor(ObjectReflector* _desc) { \
using T = type; \
_desc->name = #type; \
_desc->size = sizeof(T);

#define REFLECT_START_SUPER(type) \
ObjectReflector type::reflect { &type::initTypeDescriptor, TypeResolver<type::super>::get() }; \
__REFLECTION_START(type)

#define REFLECT_START_SUPER_MULTI(type, ...) \
ObjectReflector type::reflect { &type::initTypeDescriptor, TypeResolver<type::super>::get(), __VA_ARGS__ }; \
__REFLECTION_START(type)

#define REFLECT_START(type) \
ObjectReflector type::reflect { &type::initTypeDescriptor }; \
__REFLECTION_START(type)

//static void register_Reflect() { auto obj = new reflectObject(); TypeManager::registerObject(#type, obj);
#define REFLECT_METHOD(method) _desc->registerMethod(#method, &T::method);
#define REFLECT_FUNCTION(func) _desc->registerFunction(#func, new MethodReflector(&T::func));
#define REFLECT_PROPERTY(prop) _desc->registerProperty<decltype(T::prop)>(#prop, offsetof(T, prop));
#define REFLECT_END }
#define REFLECT  \
static void initTypeDescriptor(ObjectReflector* obj); \
static ObjectReflector reflect; \
void* operator new(size_t size);  \
virtual ObjectReflector* getReflector() { return &reflect; };

#define REFLECT_INTERFACE  \
static void initTypeDescriptor(ObjectReflector* obj); \
static ObjectReflector reflect; \
void* operator new(size_t size) = delete;