#pragma once
#include "TestClass.h"
#include "CObject.h"

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


class GCObject : public CObject {
public:
	using super = CObject;
	long long any;
	GCObject() {
		//GarbageCollector::registerObject(this);
	}


	void one() {
		any += 200;

		//std::cout << (int)GET_TAG(this)->age << ", " << (int)GET_TAG(this)->regionID << ", " << any << ", " << this << '\n';
	}
public:

	REFLECT
};


class GCObjectable : public GCObject {
public:
	using super = GCObject;
public:
	GCObject* buddy;
	GCObject* buddy2;
	GCObjectable* dude;
	REFLECT
};


class poss {
public:
	using super = void;
	int truea;
	void one() {
		std::cout << "y cccccc";
	}
private:
	virtual void overrid(int a) {
		std::cout << "base poss\n";
	}
	static void ugh();

public:
	poss() {}
	static poss* New() {
		auto a = (new (poss)());
		return new poss();
	}

	REFLECT
};

class OK : public poss {
public:
	using super = poss;
	void overrid(int a) override {
		std::cout << "overrided OK\n";
	}
	REFLECT
};


class OKDobule : public OK {
public:
	using super = OK;
	void overrid(int a) override {
		std::cout << "overrided Dobule\n";
	}
	void zep(int jan, OK* other) {
		std::cout << jan << " zaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaap\n";
	}
	REFLECT
};
