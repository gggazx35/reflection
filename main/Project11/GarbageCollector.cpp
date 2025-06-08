#include "GarbageCollector.h"
#include <iostream>
#include <thread>
#include "ThreadPool.h"


#include "CObject.h"

GarbageCollector::GarbageCollector()
{
	gray.reserve(100);
	for (int i = 0; i < regions.size()-1; i++) {
		//pushUnused(i);
		unusedRegions.push_back(i);
		//regions[i].memory = new char[MAX_REGION_CAPACITY];
		regions[i].memory = ((char*)(memoryHanlde) + (i * MAX_REGION_CAPACITY));
		regions[i].liveNodes.reserve(200);
		//regions[i].liveNodes = new void*[64];
	}
	
	eden = popUnused();
	nextStep = EGCStep::NON_GC;


	std::thread* markThread = new std::thread([this]() {
		this->gcThread();
		});
	m_thread = markThread;
	markThread->detach();
}

void GarbageCollector::mark() {
	//pushUnused(eden);
	//youngRegions.push_;back(eden);

	eden = popUnused();
#ifdef _DEBUG
	std::cout << "mark started\n";
#endif
	for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref);
			//match[ref->ptr].push_back(&ref->ptr);
			referenceMatch[ref->get()].push_back(ref);
		}
	}
	nextStep = EGCStep::CONCURRENT_MARK;
	//eden = popUnused();
}

void GarbageCollector::concurrentMark() {
	std::cout << "concurrent mark started\n";
	grayOut();
}

void GarbageCollector::finMark() {

	std::cout << "final mark started\n";
	for (auto ref : dirtyCard) {
		if(ref)
			markRef(ref);
	}
	dirtyCard.clear();
	//grayOut();
}

void GarbageCollector::markRef(CObject* _this) {
	//_this->disableRemark();
	CObject* self = _this;
	auto refs = self->getReflector();
	if (!refs) return;


	auto pointers = refs->pointers;

	for (auto ref : pointers) {
		GCPointer* val = ref->As<GCPointer>(self);
		if (nullptr != val->get()) {
			//registerGray(val);
			//registerGray(val)
			//val->ptr = GET_TAG(val->get())->forwardPointer;
			referenceMatch[val->get()].push_back(val);
			if (val->get()->state == EGCState::WHITE) {
				markRef(val->get());
			}
		}
	}
	if (!self) {
		printf("errrrrror");
		abort();
	}
	pushLive(self);
	//std::cout << "name was " << refs->name << "\n";
	//gray.pop_front();
}


void GarbageCollector::grayOut()
{
	//int graySize = _graySize;
	//auto i = gray.size();{
	//onMarking = true;
	//std::mutex m;
	{
		//ThreadPool threads(MAX_GC_THREAD);
		int graySize = 0;
		int i = 0;
		do {
			//grayStart = graySize;
			graySize = gray.getSize();
			for (; i < graySize; i++) {
				CObject* curr = gray[i];
				this->markRef(curr);
			}
			if (graySize < gray.getSize())
				continue;
		} while (0);
	}
	
	gray.reset();
	//if (gray.empty()) sweep2();
	//gray.clear();
}

void GarbageCollector::registerGray(GCPointer* val)
{
	//EGCState state = GET_TAG(val)->state;
	//int region = GET_TAG(val)->regionID;
	void* ptr = val->get();
	//std::cout << GET_TAG(val->get())->forwardPointer << ", " << val->ptr << ", " << (unsigned char)GET_TAG(val->get())->state << '\n';

	//if (GET_TAG(ptr)->state == EGCState::WHITE) {
		gray.append((CObject*)ptr);

		//GET_TAG(ptr)->state = EGCState::GRAY;
	//}
}

void GarbageCollector::startGC()
{

	time_t start, finish;
	//double duration;

	start = clock();
	runCurrentStep();

	finish = clock();
#ifdef _DEBUG
	std::cout << "gc taks " << finish - start << std::endl;
#endif
}

void GarbageCollector::pushUnused(int region)
{
	sweepRegions.erase(region);
	unusedRegions.push_back(region);
	assert(unusedRegions.size() <= 20);
	regions[region].age = 0;
	regions[region].usedSize = 0;
	regions[region].liveNodes.reset();
}

void GarbageCollector::mainMark()
{
	std::thread markThread();
	while (!gray.empty()) {
		
		grayOut();
	}
}

//Region* GarbageCollector::newRegion()
//{
//	Region* newRegion = new Region;
//	unusedRegions[0];
//	allocatedMemory += MAX_REGION_CAPACITY;
//
//	return newRegion;
//}

int GarbageCollector::popUnused()
{
	//Region* re = nullptr;
	if (unusedRegions.empty()) {
		terminate();
	}
	auto re = unusedRegions.front();
	unusedRegions.pop_front();
	sweepRegions.insert(re);
	return re;
}

int GarbageCollector::popLiveOrUnused()
{
	if (!liveRegions.empty()) {
		int region = liveRegions.front();
		return region;
	}
	return popUnused();
}

int GarbageCollector::peekUnsued()
{
	//Region* re = nullptr;
	if (unusedRegions.empty()) {
		terminate();
	}
	return unusedRegions.front();
}

void GarbageCollector::sweep2(SweepData& data, std::mutex& m) {
	AtomicArray<void*>& liveList = regions[data.fromRegion].liveNodes;
	
	int i = regions[data.fromRegion].liveNodes.getSize();
	int age = regions[data.fromRegion].age;

	i--;
	while (i >= 0) {
		void* ref = liveList[i];
		auto obj = GET_TAG(ref);
		if (obj->state == EGCState::BLACK) {
#ifdef _DEBUG 
			//std::cout << (int)obj->age << " " << obj->size << " is alive\n";
#endif

			obj->state = EGCState::WHITE;

			if ((regions[data.toRegion].usedSize + obj->getReflector()->getSize()) >= MAX_REGION_CAPACITY) {
				regions[data.toRegion].isFull = true;

				m.lock();
				if(data.toRegion == peekUnsued()) popUnused();
				data.toRegion = peekUnsued();
				m.unlock();
			}

			move(obj, data.toRegion);

		}
//#ifdef _DEBUG 
//		else if (obj->state == EGCState::WHITE)
//				std::cout << ((unsigned int)obj->state) << " has been deleted now size is " << obj->size << '\n';
//		}
//#endif
		i--;
	}

	data.liveIndex = 0;
	regions[data.fromRegion].liveNodes.reset();
}

void GarbageCollector::cleanUp(int region)
{
	char* memory = regions[region].memory;
	char* current = memory;
	char* end = memory + regions[region].usedSize;
	CObject* obj;
	while (current < end) {
		obj = GET_TAG(current);
		current += obj->getReflector()->getSize();

		if (obj->state != EGCState::BLACK)
			delete obj;
		else
			obj->state = EGCState::WHITE;
	}

}

void GarbageCollector::cleanUpRegions()
{
	for (auto region : dec) {
		cleanUp(region.fromRegion);
		pushUnused(region.fromRegion);

		if (region.toRegion == peekUnsued()) popUnused();

		if (regions[region.toRegion].usedSize <= 0)
			pushUnused(region.toRegion);
		else {
		//	std::cout << "region id: " << regions[region.toRegion].usedSize << " live\n";
		}
	}
}

void GarbageCollector::concurrentCleanUp()
{

	std::cout << "concurrent cleanup started\n";
	cleanUpRegions();
}

void GarbageCollector::gcThread()
{
	while (1) {
		if (nextStep == EGCStep::CONCURRENT_MARK) {
			concurrentMark();
			nextStep = EGCStep::FINAL_MARK;
		}

		if (nextStep == EGCStep::CONCURRENT_CLEANUP) {
			concurrentCleanUp();

			nextStep = EGCStep::NON_GC;
			//nextStep = EGCStep::FINAL_CLEANUP;
		}
	}
}

void GarbageCollector::concurrentSweep()
{
	nextStep = EGCStep::RUNNING;
	this->sweep();
	nextStep = EGCStep::CONCURRENT_CLEANUP;
	
}

void GarbageCollector::sweep() {

	//int i = youngRegions.size();
	//sdec;
	dec.clear();
	for (auto region : sweepRegions) {
		dec.push_back(SweepData(region, -1));
		//else {
		//	regions[region].liveNodes.reset();
		//}
	}

	
	{
		//int sweepSize = sweepRegions.size();
		std::mutex m;
		ThreadPool threads(MAX_GC_THREAD);
		int initialToRegion = peekUnsued();
		for (auto& region : dec) {

			region.toRegion = initialToRegion;
			threads.EnqueueJob([this, &region, &m]() {
				//region.toRegion = toRegion;
				this->sweep2(region, m);
				});
		}

		

	}

	eden = popUnused();

	referenceMatch.clear();
	nextStep = EGCStep::FINAL_CLEANUP;
}

void* GarbageCollector::Allocate(size_t _size, ObjectReflector* _reflector) {
	int size = _size;
	CObject* v = nullptr;
	if (size < DEFAULT_PADDING) size = DEFAULT_PADDING;

	if (nextStep == EGCStep::FINAL_MARK || nextStep == EGCStep::INCREASEMENTAL_SWEEP) {
		runCurrentStep();
	}

	/*if (ACTUAL_SIZEOF(size) >= MAX_REGION_CAPACITY / 2) {
		v = (AllocObj*)malloc(ACTUAL_SIZEOF(size));
	}*/
	
	while ((regions[eden].usedSize + size) >= MAX_REGION_CAPACITY) {
		runCurrentStep();
	}

	v = reinterpret_cast<CObject*>(regions[eden].memory + (regions[eden].usedSize));
	//v->forwardPointer = GET_OBJ(v);
	//v->age = 0;
#ifdef _DEBUG
	//std::cout << v << " " << v->size << '\n';
#endif
	regions[eden].usedSize += size;
	return reinterpret_cast<void*>(v);
}

void* GarbageCollector::move(CObject* tag, int toRegion)
{

	unsigned int size = tag->getReflector()->getSize();
	
	
	
	/*if ((regions[toRegion].usedSize + size) >= MAX_REGION_CAPACITY) {
		regions[toRegion].isFull = true;
		return nullptr;
	}*/
	//tag->regionID = toRegion;

	void* exAddr = tag;
	void* newAddr = currentRegionAddress(toRegion);

	memcpy(newAddr, exAddr, size);

	GET_TAG(newAddr)->state = EGCState::WHITE;
	regions[toRegion].usedSize.fetch_add(size);

	tag->fwdPtr = (CObject*)newAddr;
	GET_TAG(newAddr)->fwdPtr = (CObject*)newAddr;

	if (referenceMatch.count(tag) != 0) {
		std::vector<GCPointer*>& refVec = referenceMatch.at(tag);
		
		if (refVec.empty()) {
			abort();
		}

		for (auto ref : refVec) {
			std::cout << "ex= " << ref->ptr << "now= " << newAddr << '\n';
			ref->ptr = GET_TAG(newAddr);
		}
		refVec.clear();
	}
	else {
		abort();
	}

	//std::cout << "faushifyha " << (unsigned int)reinterpret_cast<AllocObj*>(newAddr)->forwardPointer << '\n';
	return newAddr;
}

void GarbageCollector::pushLive(CObject* live) {
	GET_TAG(live)->state = EGCState::BLACK;
	int rid = getRegion(live);
	regions[rid].pushLive((void*)live);

}

void GarbageCollector::insertDirtyCard(CObject* _changedRef) {
	//if (GET_TAG(_changedRef)->state == EGCState::WHITE)
	_changedRef->state = EGCState::GRAY;
	dirtyCard.push_back(_changedRef);
	//GET_TAG(_changedRef)->state = EGCState::WHITE;
	//_changedRef->disableRemark();
}

void GarbageCollector::runCurrentStep()
{
	switch (nextStep)	
	{
	case EGCStep::NON_GC:
		mark();
		nextStep = EGCStep::CONCURRENT_MARK;
		break;
	case EGCStep::FINAL_MARK:
		nextStep = EGCStep::RUNNING;
		finMark();
		nextStep = EGCStep::INCREASEMENTAL_SWEEP;
		break;
	case EGCStep::RUNNING:
		break;
	case EGCStep::INCREASEMENTAL_SWEEP:
		concurrentSweep();
		break;
	case EGCStep::CONCURRENT_CLEANUP:
		break;
	default:
		break;
	}
}

void GCPointer::fwd() {
	ptr = ptr->fwdPtr;
}
