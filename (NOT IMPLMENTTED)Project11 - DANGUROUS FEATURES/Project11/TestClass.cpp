#include "TestClass.h"

REFLECT_START(TestClass)
REFLECT_METHOD(overrid)
REFLECT_FUNCTION(New)
REFLECT_END


void TestClass::overrid(int a)
{
	std::cout << "base poss\n";
}

TestClass* TestClass::New()
{
	auto a = (new (TestClass)());
	return new TestClass();
}