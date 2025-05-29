#pragma once
#include "GarbageCollector.h"
namespace TypeCast {
	template<typename T>
	bool isCastable(GCMember<T>& _a, ObjectReflector* _b) {
		return _a->getReflector()->isAChildOf(_b);
	}

	template<typename T>
	bool isCastable(T* _a, ObjectReflector* _b) {
		return _a->getReflector()->isAChildOf(_b);
	}

	template<typename T, typename E>
	bool isCastable(T* _a, E* _b) {
		return _a->getReflector()->isAChildOf(_b->getReflector());
	}

	template<typename T, typename E>
	E* cast(T* out, E* other) {
		return isCastable(out, other) ? reinterpret_cast<E*>(out) : nullptr;
	}

	template<typename E, typename T>
	E* cast(T* out) {
		return (isCastable(out, TypeResolver<E>::get()) ? reinterpret_cast<E*>(out) : nullptr);
	}

	template<typename E, typename T>
	E* cast(GCMember<T>& out) {
		return (isCastable(out, TypeResolver<E>::get()) ? reinterpret_cast<E*>(out.get()) : nullptr);
	}
}