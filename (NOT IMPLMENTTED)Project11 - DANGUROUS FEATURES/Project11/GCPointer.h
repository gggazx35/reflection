#pragma once

class GCPointer {
public:
	void* ptr;
	bool remark;
	inline void* get() const { return ptr; }
	inline void enableRemark() { remark = true; }
	inline void disableRemark() { remark = false; }
};
