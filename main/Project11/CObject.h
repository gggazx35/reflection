#pragma once
#include "Reflection.h"

class CObject
{
public:
	using super = void;
private:
public:
	std::atomic<EGCState> state;
	CObject* fwdPtr;
	CObject() : state(EGCState::WHITE), fwdPtr(this) {
	};
	virtual ~CObject() {
		//std::cout << "sitttt!\n";
	}

	inline void operator delete(void* ptr) {
		//((CObject*)ptr)->state = EGCState::WHITE;
	}

	inline bool isMarked() {
		if (state == EGCState::BLACK) return true;
		return false;
	}

	inline bool isUnmarked() {
		if (state == EGCState::WHITE) return true;
		return false;
	}

	friend class GarbageCollector;
	friend class GCPointer;
	REFLECT
};

