#include "DeriveTest.h"

#include <list>
//#include "TestClass.h"
#include <iostream>
#include <time.h>
////#define MAX_OBJECT_SIZE 12800
////class GCObject;
////class GCPointer;
////
////
////enum class EGCState : unsigned char {
////	UNMARKED,
////	MARKED,
////	DEAD
////};
////
////class AllocObj {
////public:
////	EGCState state;
////	unsigned int size;
////	ObjectReflector* reflector;
////};
////
////char* memoryHanlde = new char[MAX_OBJECT_SIZE];
////size_t allocatedMemory = 0;
////std::unordered_map<void*, void*> match;
////size_t memoryMatchIdx = 0;
////
////size_t currentKill = 0;
////
////#define ACTUAL_SIZEOF(size) (size + sizeof(AllocObj))
////#define GET_TAG(x) reinterpret_cast<AllocObj*>(((char*)x) - sizeof(AllocObj))
////#define GET_OBJ(x) reinterpret_cast<void*>(((char*)x) + sizeof(AllocObj))
////#define GET_REFLECTOR(x) GET_TAG(x)->reflector
////void* Allocate(size_t size);
////
////////
////////unsigned char ObjectReflector::reflation[20][20] = {};
////////int ObjectReflector::refl_index = 0;
////////std::unordered_map<std::string, ObjectReflector*> TypeManager::objectReflections;
////
////class GarbageCollector {
////public:
////	static std::list<GCPointer*> refs;
////	static size_t allocatedSize;
////	static void mark();
////	static void markRef(void* _this);
////	static void compactRef(void* _this);
////	static void sweep();
////	static void compact();
////	static void registerObject(GCObject* object);
////};
////
////void* Allocate(size_t size) {
////	if ((allocatedMemory + size) >= MAX_OBJECT_SIZE) {
////		GarbageCollector::mark();
////		GarbageCollector::sweep();
////		GarbageCollector::compact();
////	}
////	
////	auto v = reinterpret_cast<AllocObj*>(memoryHanlde + (allocatedMemory));
////	v->size = ACTUAL_SIZEOF(size);
////	v->state = EGCState::UNMARKED;
////#ifdef _DEBUG
////	std::cout << v << " " << v->size << '\n';
////#endif
////	allocatedMemory += ACTUAL_SIZEOF(size);
////	return reinterpret_cast<void*>(((char*)v) + sizeof(AllocObj));
////}

class GCObject {
public:
	using super = void;
	long long any;
	GCObject() {
		//GarbageCollector::registerObject(this);
	}


	void one() {
		any += 200;
		
		std::cout << (int)GET_TAG(this)->age << ", " << (int)GET_TAG(this)->regionID << ", " << any << ", " << this << '\n';
	}
public:

	REFLECT
};

REFLECT_START(GCObject)
REFLECT_END

class GCObjectable : public GCObject {
public:
	using super = GCObject;
public:
	GCMember<GCObject> buddy;
	GCMember<GCObject> buddy2;
	GCMember<GCObjectable> dude;
	REFLECT
};

REFLECT_START_SUPER(GCObjectable)
REFLECT_PROPERTY(buddy)
REFLECT_PROPERTY(buddy2)
REFLECT_PROPERTY(dude)
REFLECT_END

class poss {
public:
	using super = void;
	int truea;
	void one() {
		std::cout << "y cccccc";
	}
private:
	virtual void overrid(int a) {
		std::cout << "base poss\n";
	}
	static void ugh();

public:
	poss() {}
	static poss* New() {
		auto a = (new (poss)());
		return new poss();
	}

	REFLECT
};
REFLECT_START(poss)
	REFLECT_METHOD(overrid)
	REFLECT_FUNCTION(New)
	REFLECT_PROPERTY(truea)
REFLECT_END

class OK : public poss {
public:
	using super = poss;
	void overrid(int a) override {
		std::cout << "overrided OK\n";
	}
	REFLECT
};

REFLECT_START_SUPER(OK)
REFLECT_END

class OKDobule : public OK {
public:
	using super = OK;
	void overrid(int a) override {
		std::cout << "overrided Dobule\n";
	}
	void zep(int jan, OK* other) {
		std::cout << jan << " zaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaap\n";
	}
	REFLECT
};

REFLECT_START_SUPER(OKDobule)
REFLECT_METHOD(zep)
REFLECT_END
//
//template<typename R, typename... TARGS>
//R call(MethodReflector* vPtr, TARGS... args) {
//	using func = R(*)(TARGS...);
//	//assert(typeid(func).hash_code() == vPtr->hash);
//	
//	return (reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
//	
//	//(reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
//}

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


typedef poss stupid;
int main() {
	auto gco = GCPtr<GCObjectable>(new GCObjectable());
	auto gcob = GCPtr<GCObjectable>(new GCObjectable());
	//std::cout << "sizeof" << gco->getReflector()->size;
	std::cout << TypeResolver<Derive2Test>::get()->isSuperOf(TypeResolver<DeriveTest>::get()) << '\n';
	std::cout << TypeResolver<DTest>::get()->isAChildOf(TypeResolver<TestClass>::get()) << '\n';
	std::cout << TypeResolver<Derive2Test>::get()->isAChildOf(TypeResolver<DeriveTest>::get()) << '\n';
	std::cout << TypeResolver<TTest>::get()->isAChildOf(TypeResolver<TestClass>::get()) << '\n';



	std::cout << TypeResolver<Derive2Test>::get()->isSame(TypeResolver<DeriveTest>::get()) << '\n';
	std::cout << TypeResolver<DTest>::get()->isChildOf(TypeResolver<DeriveTest>::get()) << '\n';
	std::cout << TypeResolver<Derive2Test>::get()->isSuperOf(TypeResolver<DeriveTest>::get()) << '\n';
	std::cout << TypeResolver<TTest>::get()->isAChildOf(TypeResolver<TestClass>::get()) << '\n';

	time_t start, finish;
	//double duration;

	start = clock();
	gco->dude = new GCObjectable();
	//gcob = new GCObjectable();
	gcob->one();
	gco->dude->one();
	for (int i = 0; i < 200000; i++) {
		//gco->dude = new GCObjectable();
		//if (i == 0) gcob.ptr = gco->dude;
		gco->buddy = new GCObjectable();
		gco->buddy = new GCObjectable();
		gco->buddy2 = new GCObject();
		if (i == 5000) gco->dude = (GCObjectable*)gco->buddy.get();
		//std::cout << "size is" << GET_TAG(gco->buddy)->size << "\n\n\n\n\n";
		//if (GET_REFLECTOR(gco.ptr)->isAChildOf(GET_REFLECTOR(gco->buddy2))) {
		
		//}
		//gcob->one();
	}
	gcob->one();
	gco->dude->one();
	auto pro = GCPtr<TestClass>(new TestClass());
	pro->truea = 20;
	finish = clock();
	
	std::cout << TypeResolver<poss>::get()->name << '\n';
	std::cout << TypeResolver<OK>::get()->isChildOf(TypeResolver<poss>::get()) << '\n';
	std::cout << TypeResolver<OK>::get()->isSuperOf(TypeResolver<poss>::get()) << '\n';
	std::cout << GET_TAG(gco.ptr)->reflector->name << '\n';

	//std::cout << "casted " << cast<poss>(newPoss) << '\n';
	//std::cout << "cast failed because the newPoss is not child of OkDobule " << cast<OKDobule>(newPoss) << '\n';

	//r(newPoss);
	//typeid(poss).hash_code();
	//call<void>(r, newPoss);
	//std::vector<int> ap(20, 20);
	//auto e = new poss;
	//int (*thig)(poss*, int io2) = [](poss* self, int io2) {
		//self->one();


	//delete[] memoryHanlde;
	//typedef int r;
	return 0;
}
