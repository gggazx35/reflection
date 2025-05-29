#pragma once
#include "ObjectReflector.h"
#include <deque>
#include <array>
#include <unordered_set>
#include <atomic>
#include <mutex>
#include <cassert>
#define MAX_OBJECT_SIZE 25600000
#define MAX_REGION_CAPACITY (MAX_OBJECT_SIZE / 20)
#define GC_OBJECT_SIZE (MAX_OBJECT_SIZE - 800)
#define DEFAULT_PADDING 8
#define MAX_GC_THREAD 1

enum class EGCState : unsigned char {
	NONE,
	WHITE,
	GRAY,
	BLACK
};

enum class EGCStep : unsigned char {
	NON_GC,
	RUNNING,
	FINAL_MARK,
	FINAL_CLEANUP,
};

class AllocObj {
public:
	volatile EGCState state;
	unsigned char age;
	unsigned short regionID;
	unsigned int size;
	ObjectReflector* reflector;
	void* forwardPointer;
};

class SweepCell {
	int region;
};

#define ACTUAL_SIZEOF(size) (size + sizeof(AllocObj))
#define GET_TAG(x) reinterpret_cast<AllocObj*>(((char*)x) - sizeof(AllocObj))
#define GET_OBJ(x) reinterpret_cast<void*>(((char*)x) + sizeof(AllocObj))
#define GET_REFLECTOR(x) GET_TAG(x)->reflector

template<typename T>
struct AtomicArray {
private:
	T* data;
	std::atomic<size_t> size;
public:
	AtomicArray<T>() {
		data = nullptr;
		size.store(0);
	}
	AtomicArray<T>(size_t initalSize) {
		data = new T[initalSize];
		size.store(0);
	}
	~AtomicArray<T>() {
		if(data) delete[] data;
	}

	void reserve(size_t newSize) {
		if(data) delete[] data;
		data = new T[newSize];
	}

	inline void append(T value) {
		data[size.load()] = value;
		size.fetch_add(1);
	}
	inline size_t getSize() {
		return size.load();
	}
	inline T& operator[](size_t idx) {
		return data[idx];
	}
	inline void reset() {
		size.store(0);
	}
	inline bool empty() {
		return size.load() == 0;
	}
};


template<typename T>
class QPtr {
	T* ptr;

};

class Region {
public:
	char* memory;
	std::atomic<int> usedSize;
	int age;
	bool isFull;
	//void** liveNodes;
	AtomicArray<void*> liveNodes;

	inline void pushLive(void* _live) {
		liveNodes.append(_live);
	}

	//std::vector<void*> grayNodes;
	//bool unused;
	////Region(char* _mem) : memory(_mem), usedSize(0) {
	////	//unused = true;
	////	age = 0;
	////}
};



class GarbageCollector {
private:
	GarbageCollector();
	GarbageCollector(const GarbageCollector& ref) {}
	GarbageCollector& operator=(const GarbageCollector& ref) {}
	~GarbageCollector() {
		free(memoryHanlde);
	}
	struct SweepData {
		int fromRegion;
		int toRegion;
		int liveIndex;
		SweepData(int from, int to) : fromRegion(from), toRegion(to), liveIndex(0)  {
			
		}
	};
	/// WARNING: DON'T USE THIS FOR ALLOCATING MEMORY 
	char* memoryHanlde = new char[MAX_REGION_CAPACITY * 20];
public:/*
	char* oldGeneration = new char[MAX_OBJECT_SIZE];
	char* surviorGeneration = new char[MAX_OBJECT_SIZE];*/

	// INFO: regions are splited memoryHandle usually uses when allocating memory referencing liveNodes
	std::array<Region, 20> regions;
	
	// WARNING: NEVER USE THIS FOR SEARCHING NON-ALLOCATED REGIONS USE 'popUnused()' instead
	std::deque<int> unusedRegions;
	std::deque<int> liveRegions;
	std::deque<int> fullRegions;
	std::deque<SweepData> dec;
	//std::deque<int> markedRegions;
	//size_t allocatedMemory = 0;
	//std::unordered_map<int, void**> liveNodes;
	
	// INFO: current eden's region id
	int eden;
	
	// (NOT IMPLMENTED) INFO: current survivor's region id
	int survivor;
	

	// INFO: this re-matching allocated objects from old address to new address only uses while compacting memory
	//std::unordered_map<void*, std::list<void**>> match;
	// INFO: this re-matching allocated objects in reference list from old address to new address only uses while compacting memory
	std::unordered_map<void*, std::vector<GCPointer*>> referenceMatch;


	std::unordered_set<int> sweepRegions;
	std::list<GCPointer*> refs;
	std::list<void*> dirtyCard;
	//std::deque<GCPointer*> gray;
	AtomicArray<void*> gray;
	//std::deque<void*> live;
	//std::list<void*> black;
	//std::list<void*> live;
	//std::thread markingThread;

	EGCStep nextStep;

	bool onGC = false;
	bool onMarking = false;
	bool onSweep;
	// stw
	void mark();
	void concurrentMark();
	void finMark();
	// concurrent
	void markRef(void* _this);


	void concurrentSweep();
	void sweep();
	void sweep2(SweepData& data, std::mutex& m);

	void cleanUpRegions();

	void grayOut();




	void startGC();

	// idk
	void compact();
	void compactRef(void* _this);
	
	void pushUnused(int region);
	void mainMark();
	int popUnused();
	int popLiveOrUnused();
	int peekUnsued();

	void registerGray(GCPointer* val);


	void* Allocate(size_t size);
	void* move(AllocObj* tag, int toRegion);

	inline void* currentRegionAddress(int region) {
		return (regions[region].memory + regions[region].usedSize);
	}

	inline void pushLive(void* live) {
		GET_TAG(live)->state = EGCState::BLACK;
		int rid = GET_TAG(live)->regionID;
		//GET_TAG(live)->state = EGCState::WHITE;
		regions[rid].pushLive(live);

		//std::cout << live << "is Alive\n";
	}

	inline void insertDirtyCard(void* _changedRef) {
		dirtyCard.push_back(_changedRef);
		//GET_TAG(_changedRef)->state = EGCState::WHITE;
		//_changedRef->disableRemark();
	}

	void runCurrentStep();

	static GarbageCollector* get() {
		static GarbageCollector instance;
		return &instance;
	}
};

template<typename T>
class GCMember : public GCPointer {
public:
	GCMember<T>() {
		ptr = nullptr;
		remark = true;
	}

	inline GCMember<T>& operator=(const GCMember<T>& other) {
		if (GarbageCollector::get()->onMarking) {
			if (ptr != nullptr) {
				GarbageCollector::get()->insertDirtyCard(this->get());
				std::cout << static_cast<int>(GET_TAG(this->get())->state) << '\n';
			}
		}
		ptr = reinterpret_cast<void*>(other.get());
		ptr = GET_TAG(ptr)->forwardPointer;
		return *this;
	}

	inline GCMember<T>& operator=(T* other) {
		if (GarbageCollector::get()->onMarking) {
			if (ptr != nullptr) {
				ptr = GET_TAG(ptr)->forwardPointer;
				//GarbageCollector::get()->insertDirtyCard(this->get());
				//std::cout << static_cast<int>(GET_TAG(this->get())->state) << '\n';
			}
		}
		ptr = reinterpret_cast<void*>(other);
		if(ptr) ptr = GET_TAG(ptr)->forwardPointer;
		return *this;
	}

	inline bool operator==(T* other) {

		ptr = GET_TAG(ptr)->forwardPointer;
		other = GET_TAG(other)->forwardPointer;
		return ptr == other;
	}

	inline bool operator!=(T* other) {
		return !(this->operator==(other));
	}

	inline bool operator==(const GCMember<T>& other) {
		return ptr == (T*)other.get();
	}

	inline bool operator!=(const GCMember<T>& other) {
		return !(this->operator==(other));
	}

	inline T* operator->() {
#ifdef _DEBUG
		//if (!ptr) assert("nullptr referenced");
		if (!GET_TAG(ptr)->forwardPointer) assert("nullptr referenced");
#endif
		ptr = GET_TAG(ptr)->forwardPointer;
		return reinterpret_cast<T*>(ptr);
	}
};

template<typename T>
class GCPtr : public GCMember<T> {
public:
	GCPtr<T>(T* _ptr) {
		this->ptr = reinterpret_cast<void*>(_ptr);
		GarbageCollector::get()->refs.push_back(this);
	}

	~GCPtr<T>() {
		GarbageCollector::get()->refs.remove(this);
	}
};

