#pragma once
#include <vector>

template<typename T>
struct MethodConverter;

template<typename R, typename T, typename... TARGS>
struct MethodConverter<R(T::*)(TARGS...)> {
	using methodType = R(T::*)(TARGS...);
	auto convert(methodType func_) {
		static methodType doubla = 0;
		doubla = func_;
		R(*deepshit)(T*, TARGS...) = [](T* self, TARGS... args) {
			(self->*doubla)(args...);
			};
		return deepshit;
	}

};


class MethodReflector {
public:
	void* ptr;
	size_t hash;
	//std::vector<size_t> parameters;

	template<typename R, typename... TARGS>
	MethodReflector(R(*func)(TARGS...)) {
		ptr = reinterpret_cast<void*>(func);
		hash = typeid(func).hash_code();
		//for (int i = 0; i < sizeof...(TARGS...); i++) {
		//parameters = { typeid(TARGS).hash_code()... };

		/*size_t x[] = { typeid(TARGS).hash_code()... };
		parameters = x;
		parameters = (size_t*)malloc(sizeof(x));
		memcpy(parameters, x, sizeof(x));*/
	}
	~MethodReflector() {

	}
};


template<typename R, typename... TARGS>
R call(MethodReflector* vPtr, TARGS... args) {
	using func = R(*)(TARGS...);
	return (reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
}