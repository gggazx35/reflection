#pragma once
#include <atomic>
class GCPointer {
public:
	void* ptr;
	std::atomic<bool> remark;
	inline void* get() const { return ptr; }
	inline void enableRemark() { remark = true; }
	inline void disableRemark() { remark = false; }
};
