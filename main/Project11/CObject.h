#pragma once
#include "Reflection.h"

enum class EGCState : unsigned char {
	NONE,
	WHITE,
	GRAY,
	BLACK,
	DEAD
};

class CObject
{
public:
	using super = void;
private:
	EGCState state;
public:
	CObject() : state(EGCState::WHITE) {
		state = EGCState::WHITE;
	};
	virtual ~CObject() {
		//std::cout << "sitttt!\n";
	}

	inline void operator delete(void* ptr) {
		//((CObject*)ptr)->state = EGCState::WHITE;
	}

	friend class GarbageCollector;
	REFLECT
};

