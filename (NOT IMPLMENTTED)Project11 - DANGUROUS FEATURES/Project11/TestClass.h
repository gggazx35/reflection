#pragma once
#include "Reflection.h"
#include <iostream>


class TestClass {
public:
	using super = void;
	int truea;
	void one() {
		std::cout << "y cccccc";
	}
private:
	virtual void overrid(int a);

public:
	TestClass() {}
	static TestClass* New();

	REFLECT
};
