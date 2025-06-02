#include "DeriveTest.h"
#include "TypeCast.h"
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


typedef poss stupid;
int main() {
	GCPtr<GCObjectable> gco(new GCObjectable());
	GCPtr<GCObjectable> gcob(new GCObjectable());
	
	time_t start, finish;
	//double duration;

	start = clock();
	gco->dude = new GCObjectable();
	//gcob = new GCObjectable();
	gcob->one();
	gco->dude->one();

	std::cout << TypeResolver<poss>::get()->name << '\n';
	std::cout << TypeResolver<OK>::get()->isChildOf(TypeResolver<poss>::get()) << '\n';
	std::cout << TypeResolver<OK>::get()->isSuperOf(TypeResolver<poss>::get()) << '\n';
	std::cout << TypeCast::cast<GCObject>(new GCObjectable) << '\n';

	if (TypeCast::cast<GCObject>(new GCObjectable)) {
		printf("cast into GCObject\n");
	}
	for (int i = 0; i < 10000000; i++) {
		//gco->dude = new GCObjectable();
		//if (i == 0) gcob.ptr = gco->dude;
		gco->buddy = new GCObjectable();
		gco->buddy = new GCObjectable();
		gco->buddy2 = new GCObject();
		if (i == 5000) gco->dude = TypeCast::cast<GCObjectable>(gco->buddy);

		gco->buddy = new GCObjectable();
		gco->buddy = new GCObjectable();
		gco->buddy2 = new GCObject();

		gco->buddy = new GCObjectable();
		gco->buddy = new GCObjectable();
		gco->buddy2 = new GCObject();
		//std::cout << "size is" << GET_TAG(gco->buddy)->size << "\n\n\n\n\n";
		//if (GET_REFLECTOR(gco.ptr)->isAChildOf(GET_REFLECTOR(gco->buddy2))) {
		//}
		//gcob->one();
	}
	gcob->one();
	gco->dude->one();

	GCPtr<TestClass> pro(new TestClass());
	pro->truea = 20;
	finish = clock();
	
	std::cout << "allocation done by " << finish - start << "\n";
	//std::cout << GET_TAG(gco.ptr)->reflector->name << '\n';

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
