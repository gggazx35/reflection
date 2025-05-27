#pragma once
#include "TestClass.h"

class IImp {
	using super = void;
public:
	REFLECT
};

class DeriveTest : public TestClass
{
	using super = TestClass;
	void overrid(int a) override;
public:
	REFLECT
};

class Derive2Test : public DeriveTest
{
	using super = DeriveTest;
	void overrid(int a) override;
public:
	REFLECT
};

class DTest : public DeriveTest
{
	using super = DeriveTest;
	void overrid(int a) override;
public:
	REFLECT
};

class TTest : public DeriveTest, public IImp
{
	using super = DeriveTest;
	void overrid(int a) override;
public:
	REFLECT
};