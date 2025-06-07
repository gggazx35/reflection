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
}

void GarbageCollector::mark() {
	//pushUnused(eden);
	//youngRegions.push_;back(eden);

	eden = popUnused();
	referenceMatch.clear();
#ifdef _DEBUG
	std::cout << "mark started\n";
#endif
	for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref);
			//match[ref->ptr].push_back(&ref->ptr);
			//referenceMatch[ref->ptr].push_back(ref);
		}
	}
	onMarking = true;
	//eden = popUnused();
}

void GarbageCollector::concurrentMark() {
	std::thread markThread([this]() { 
		this->grayOut(); 
		onMarking = false; 
		nextStep = EGCStep::FINAL_MARK;
		});
	markThread.join();
}

void GarbageCollector::finMark() {

	/*for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref);
		}
	}
*//*
	for (auto ref : dirtyCard) {
		if(ref)
			markRef(ref);
	}
	dirtyCard.clear();*/
	//grayOut();
}

void GarbageCollector::markRef(CObject* _this) {
	//_this->disableRemark();
	CObject* self = _this;
	auto refs = self->getReflector();
	if (!refs) return;


	auto pointers = refs->pointers;

	for (auto ref : pointers) {
		void** val = ref->As<void*>(self);
		if (nullptr != (*val)) {
			//registerGray(val);
			//registerGray(val)
			//val->ptr = GET_TAG(val->get())->forwardPointer;
			if (GET_TAG(*val)->state == EGCState::WHITE) {
				referenceMatch[*val].push_back(val);
				markRef(reinterpret_cast<CObject*>(*val));
			}
		}
	}
	if (!self) {
		printf("errrrrror");
		abort();
	}
	pushLive((CObject*)self);
	//std::cout << "name was " << refs->name << "\n";
	//gray.pop_front();
}

void GarbageCollector::compact() {
	for (auto ref : refs) {
		if (ref->ptr) compactRef(ref->ptr);
	}
}

void GarbageCollector::grayOut()
{
	//int graySize = _graySize;
	//auto i = gray.size();{
	//onMarking = true;
	std::mutex m;
	{
		ThreadPool threads(MAX_GC_THREAD);
		int graySize = 0;
		int i = 0;
		do {
			//grayStart = graySize;
			graySize = gray.getSize();
			for (; i < graySize; i++) {
				CObject* curr = gray[i];
				//if (curr->remark == true) {
				threads.EnqueueJob([this, curr, &m]() { this->markRef(curr); });
				//this->markRef(curr, m);
					//gray.pop_front();
				//}
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

	if (GET_TAG(ptr)->state == EGCState::WHITE) {
		gray.append((CObject*)ptr);

		//GET_TAG(ptr)->state = EGCState::GRAY;
	}
}

void GarbageCollector::startGC()
{

	time_t start, finish;
	//double duration;

	start = clock();
	//do {

	//	if (onGC == false) {
	//		onGC = true;
	//		while (onSweep == true);
	//		mark();
	//		onMarking = true;
	//		std::thread markThread([this]() { this->grayOut(); onMarking = false; });
	//		markThread.detach();
	//	}
	//	else {
	//		while (onMarking == true);
	//		//// stop the world ////
	//		finMark();
	//		onSweep = true;
	//		sweep();
	//		onSweep = false;
	//		onGC = false;
	//		/// end stop the world ///
	//	}
	//} while (0);
	while (nextStep == EGCStep::RUNNING);
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

void GarbageCollector::compactRef(void* _this) {
	//auto refs = GET_REFLECTOR(_this);
	//for (auto ref : refs->pointers) {
	//	if (ref) {
	//		auto ptr = match[*ref->As<void*>(_this)];
	//		*ref->As<void*>(_this) = ptr;

	//		if (ptr) compactRef(ptr);
	//	}
	//}
	////std::cout << "*ref->As<void*>(this)" << '\n';
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

	//pushUnused(fromRegion);


	//if (regions[data.toRegion].usedSize > 0) {
	//	regions[data.toRegion].age = age + 1;
	//	//youngRegions.push_back(toRegion);
	//	//sweepRegions.emplace(toRegion);
	//	
	//}
	//else {
	//	//isFailure = true;
	//	//pushUnused(toRegion);
	//	//sweepRegions.erase(toRegion);
	//	//std::cout << "region id: " << data.toRegion << " has been freed\n";
	//}

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
			//std::cout << "region id: " << regions[region.toRegion].usedSize << " live\n";
		}
	}
	eden = popUnused();
}

void GarbageCollector::concurrentSweep()
{
	onSweep = true;
	std::thread markThread([this]() { 
		this->sweep(); 
		});
	markThread.join();
	
}

void GarbageCollector::sweep() {

	//int i = youngRegions.size();
	//sdec;
	dec.clear();
	for (auto region : sweepRegions) {
		//if (region != eden)
			dec.push_back(SweepData(region, -1));
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

	nextStep = EGCStep::FINAL_CLEANUP;
	/*auto& edenRefs = regions[eden].liveNodes;
	if (!edenRefs.empty()) {
		for (int i = 0; i < edenRefs.getSize(); i++) {
			GET_TAG(edenRefs[i])->state = EGCState::WHITE;
		}
	}
	edenRefs.reset();*/

	
	


	//match.clear();
	//sweepRegions.clear();
	/*for (auto m : refs) {
		m->ptr = lv[m->ptr];
	}*/
}

void* GarbageCollector::Allocate(size_t _size, ObjectReflector* _reflector) {
	int size = _size;
	CObject* v = nullptr;
	if (size < DEFAULT_PADDING) size = DEFAULT_PADDING;

	if (nextStep != EGCStep::NON_GC && nextStep != EGCStep::RUNNING) {
		runCurrentStep();
	}

	/*if (ACTUAL_SIZEOF(size) >= MAX_REGION_CAPACITY / 2) {
		v = (AllocObj*)malloc(ACTUAL_SIZEOF(size));
	}*/

	while ((regions[eden].usedSize + size) >= MAX_REGION_CAPACITY) {
		startGC();
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
	tag->state = EGCState::WHITE;
	//tag->regionID = toRegion;

	void* exAddr = tag;
	void* newAddr = currentRegionAddress(toRegion);

	memcpy(newAddr, exAddr, size);
	regions[toRegion].usedSize.fetch_add(size);


	if (referenceMatch.count(GET_OBJ(tag)) != 0) {
		std::vector<void**>& refVec = referenceMatch.at(GET_OBJ(tag));
		for (auto ref : refVec) {
			*ref = GET_OBJ(newAddr);
		}
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
	if (GET_TAG(_changedRef)->state == EGCState::WHITE)
		dirtyCard.push_back(_changedRef);
	//GET_TAG(_changedRef)->state = EGCState::WHITE;
	//_changedRef->disableRemark();
}

void GarbageCollector::runCurrentStep()
{
	switch (nextStep)	
	{
	case EGCStep::NON_GC:
		nextStep = EGCStep::RUNNING;
		mark();
		concurrentMark();
		break;
	case EGCStep::FINAL_MARK:
		nextStep = EGCStep::RUNNING;
		concurrentSweep();
		break;
	case EGCStep::FINAL_CLEANUP:
		
		cleanUpRegions();
		nextStep = EGCStep::NON_GC;
		break;
	case EGCStep::RUNNING:
		break;
	}
}
