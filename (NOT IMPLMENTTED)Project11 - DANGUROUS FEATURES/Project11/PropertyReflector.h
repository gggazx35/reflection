#pragma once

class PropertyReflector
{
	size_t hash;
	unsigned int offset;
public:
	//template<typename T>
	PropertyReflector(unsigned int _offset) {
		//hash = typeid(T).hash_code();
		offset = _offset;
	}

	template<typename E, typename T>
	E* As(T* ptr) {
		return reinterpret_cast<E*>((char*)ptr + offset);
	}
};

