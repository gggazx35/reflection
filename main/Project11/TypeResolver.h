#pragma once
#include "ObjectReflector.h"
template<class T>
struct TypeResolver {
	static ObjectReflector* get() {
		//if ((&T::reflect)->name == "") T::reflect{ &T::initTypeDescriptor };
		return &T::reflect;
	}
};

