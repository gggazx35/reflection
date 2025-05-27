#include "TestClass.h"
#include "DeriveTest.h"

REFLECT_START(IImp)
REFLECT_END

REFLECT_START_SUPER(DeriveTest)
REFLECT_END

REFLECT_START_SUPER(Derive2Test)
REFLECT_END

REFLECT_START_SUPER(DTest)
REFLECT_END

REFLECT_START_SUPER_MULTI(TTest, TypeResolver<IImp>::get())
REFLECT_END


void DeriveTest::overrid(int a)
{
	std::cout << "fool " << a << '\n';
}

void Derive2Test::overrid(int a) {

	std::cout << "idiot " << a << '\n';
}