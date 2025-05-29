#include "GarbageCollector.h"
#include <iostream>
#include <thread>
#include "ThreadPool.h"
GarbageCollector::GarbageCollector()
{
	for (int i = 0; i < regions.size()-1; i++) {
		//pushUnused(i);
		unusedRegions.push_back(i);
		//regions[i].memory = new char[MAX_REGION_CAPACITY];
		regions[i].memory = ((char*)(memoryHanlde) + (i * MAX_REGION_CAPACITY));
		//regions[i].liveNodes = new void*[64];
	}
	
	eden = popUnused();
}

//void GarbageCollector::mark() {
//	for (auto ref : refs) {
//		if (ref->ptr){ 
//			markRef(ref->ptr);
//			GET_TAG(ref->ptr)->state = EGCState::LATER;
//		}
//	}
//}
// to impl set gray when ref
void GarbageCollector::mark() {
	//pushUnused(eden);
	//youngRegions.push_;back(eden);

	eden = popUnused();
	std::cout << "mark started\n";
	for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref);
			//match[ref->ptr].push_back(&ref->ptr);
			//referenceMatch[ref->ptr].push_back(ref);
		}
	}
	//eden = popUnused();
}

void GarbageCollector::finMark() {

	for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref);
		}
	}

	for (auto ref : dirtyCard) {
		if(ref->ptr)
			registerGray(ref);
	}
	dirtyCard.clear();
	grayOut();
}

void GarbageCollector::markRef(GCPointer* _this, std::mutex& m) {
	_this->disableRemark();
	void* self = _this->ptr;
	auto refs = GET_REFLECTOR(self);
	if (!refs) return;


	auto pointers = refs->pointers;

	for (auto ref : pointers) {
		GCPointer* val = ref->As<GCPointer>(self);
		if (val->get()) {
			//registerGray(val);
			if (GET_TAG(val)->state == EGCState::WHITE) {
				registerGray(val);
			}
			//if (match.count(val) == 0) 
			/*m.lock();
			referenceMatch[val->get()].push_back(val);
			m.unlock();*/
		}
	}
	m.lock();
	GET_TAG(self)->state = EGCState::BLACK;
	pushLive(self);
	m.unlock();
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
				GCPointer* curr = gray[i];
				//if (curr->remark == true) {
				//threads.EnqueueJob([this, curr, &m]() { this->markRef(curr, m); });
				this->markRef(curr, m);
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

	val->ptr = GET_TAG(ptr)->forwardPointer;
	if (GET_TAG(ptr)->state == EGCState::WHITE) {
		gray.append(val);

		//GET_TAG(ptr)->state = EGCState::GRAY;
		val->enableRemark();
	}
}

void GarbageCollector::startGC()
{

	time_t start, finish;
	//double duration;

	start = clock();
	do {

		if (onGC == false) {
			onGC = true;
			while (onSweep == true);
			mark();
			
			std::thread markThread([this]() { onMarking = true; this->grayOut(); onMarking = false; });
			markThread.detach();
		}
		else {
			while (onMarking == true);
			//// stop the world ////
			finMark();
			onSweep = true;
			sweep();
			onSweep = false;
			onGC = false;
			/// end stop the world ///
		}
	} while (0);

	finish = clock();

	std::cout << "gc taks " << finish - start << std::endl;
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

	if (data.liveIndex != 0)
		i = data.liveIndex;
	else
		i--;
	while (i >= 0) {
		void* ref = liveList[i];
		auto obj = GET_TAG(ref);
		if (obj->state == EGCState::BLACK) {
#ifdef _DEBUG 
			//std::cout << (int)obj->age << " " << obj->size << " is alive\n";
#endif

			obj->state = EGCState::WHITE;

			if ((regions[data.toRegion].usedSize + obj->size) >= MAX_REGION_CAPACITY) {
				regions[data.toRegion].isFull = true;

				m.lock();
				if(data.toRegion == peekUnsued()) popUnused();
				data.toRegion = peekUnsued();
				m.unlock();
			}

			move(obj, data.toRegion);

		}
#ifdef _DEBUG 
		else
			std::cout << ((unsigned int)obj->state) << " has been deleted now size is " << obj->size << '\n';
#endif
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

void GarbageCollector::concurrentSweep()
{
	onSweep = true;
	std::thread markThread([this]() { this->sweep(); onSweep = false; });
	markThread.detach();
}

void GarbageCollector::sweep() {

	//int i = youngRegions.size();
	std::deque<SweepData> dec;

	for (auto region : sweepRegions) {
		if (region != eden)
			dec.push_back(SweepData(region, -1));
	}

	
	{
		//int sweepSize = sweepRegions.size();
		std::mutex m;
		ThreadPool threads(MAX_GC_THREAD);
		int initialToRegion = peekUnsued();
		for (auto& region : dec) {
			//int region = youngRegions.front();
			//youngRegions.pop_front();
			//SweepData* ptr = region;
			//region.toRegion = 2000;
			region.toRegion = initialToRegion;
			threads.EnqueueJob([this, &region, &m]() {
				//region.toRegion = toRegion;
				this->sweep2(region, m);
				});
			//sweep2(region, popUnused());
			
			//i--;
		}

		

	}

	
	for (auto region : dec) {
		pushUnused(region.fromRegion);

		if (region.toRegion == peekUnsued()) popUnused();

		if (regions[region.toRegion].usedSize <= 0)
			pushUnused(region.toRegion);
		else {
			//std::cout << "region id: " << regions[region.toRegion].usedSize << " live\n";
		}
	}
	auto& edenRefs = regions[eden].liveNodes;
	if (!edenRefs.empty()) {
		for (int i = 0; i < edenRefs.getSize(); i++) {
			GET_TAG(edenRefs[i])->state = EGCState::WHITE;
		}
	}
	edenRefs.reset();


	//match.clear();
	//sweepRegions.clear();
	/*for (auto m : refs) {
		m->ptr = lv[m->ptr];
	}*/
}

void* GarbageCollector::Allocate(size_t _size) {
	int size = _size;
	if (size < DEFAULT_PADDING) size = DEFAULT_PADDING;

	if (onGC){
		if (!onMarking) {
			finMark();
			concurrentSweep();
			onGC = false;
		}
	}

	if ((regions[eden].usedSize + ACTUAL_SIZEOF(size)) >= MAX_REGION_CAPACITY) {
		startGC();
		//sweepRegions.clear();
	}

	auto v = reinterpret_cast<AllocObj*>(regions[eden].memory + (regions[eden].usedSize));
	v->size = ACTUAL_SIZEOF(size);
	v->state = EGCState::WHITE;
	v->regionID = eden;
	v->forwardPointer = GET_OBJ(v);
	v->age = 0;
#ifdef _DEBUG
	//std::cout << v << " " << v->size << '\n';
#endif
	regions[eden].usedSize += ACTUAL_SIZEOF(size);
	return reinterpret_cast<void*>(((char*)v) + sizeof(AllocObj));
}

void* GarbageCollector::move(AllocObj* tag, int toRegion)
{

	unsigned int size = tag->size;
	
	
	/*if ((regions[toRegion].usedSize + size) >= MAX_REGION_CAPACITY) {
		regions[toRegion].isFull = true;
		return nullptr;
	}*/

	tag->regionID = toRegion;
	tag->age++;

	void* exAddr = tag;
	void* newAddr = currentRegionAddress(toRegion);

	memcpy(newAddr, exAddr, size);
	regions[toRegion].usedSize.fetch_add(size);

	reinterpret_cast<AllocObj*>(newAddr)->forwardPointer = GET_OBJ(newAddr);
	tag->forwardPointer = GET_OBJ(newAddr);
	return newAddr;
}
