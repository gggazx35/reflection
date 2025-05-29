#pragma once
#include <atomic>
class GCPointer {
protected:
	void* ptr;
public:
	std::atomic<bool> remark;
	inline void enableRemark() { remark = true; }
	inline void disableRemark() { remark = false; }
	inline void* get() const { return ptr; }
	
	friend class GarbageCollector;
};
