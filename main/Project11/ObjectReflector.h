#pragma once
#include <string>
#include <unordered_map>
#include <tuple>
#include "MethodReflector.h"
#include "PropertyReflector.h"
#include "GCPointer.h"
#include <iostream>
#define BIT(x) (1 << x)

enum EMatch : unsigned char {
	kNone = 0x0,
	kSame = BIT(1),
	kIsAChildOf = BIT(2),
	kIsASuperOf = BIT(3),
	kSuperOf = BIT(4),
	kChildOf = BIT(5)
};

class TypeManager;
class ObjectReflector {
	size_t hash;
	ObjectReflector* super;
	std::vector<ObjectReflector*> interfaces;
public:
	std::unordered_map<std::string, MethodReflector*> methods;
	std::unordered_map<std::string, PropertyReflector*> properties;
	std::list<PropertyReflector*> pointers;
	std::string name;

	unsigned int size;
	unsigned int N;
	///*template<typename Tx>
	//reflectObject(Tx x) {
	//	hash = typeid(Tx).hash_code();
	//}*/

	static unsigned char reflation[40][40];
	static int refl_index;

	//ObjectReflector(void (*init)(ObjectReflector*));
	//ObjectReflector(void (*init)(ObjectReflector*), ObjectReflector* _super=nullptr);
	template <typename... TARGS>
	ObjectReflector(void (*init)(ObjectReflector*), TARGS*... _super) {
		super = nullptr;
		if constexpr (sizeof...(TARGS) >= 0) {
			interfaces = { _super... };
			for (auto item : interfaces) {
				TypeManager::get()->requirements[item].push_back(this);
			}
			//super = interfaces[0];
		}
		
		init(this);
		
		N = refl_index++;

		if (TypeManager::get()->requirements.count(this)) {
			std::cout << name << '\n';
			markClassTree();
		}

		reflation[N][N] = EMatch::kSame;
		TypeManager::get()->objectReflections.emplace(std::make_pair(name, this));

	}
	/*static void registerObject(std::string name, ObjectReflector* refl) {
		objectReflections.insert(std::make_pair(name, refl));
	}*/

	template<typename T>
	void registerMethod(std::string str, T method) {
		MethodConverter<T> func;
		methods.insert(std::make_pair(str, new MethodReflector(func.convert(method))));
	}

	void registerFunction(std::string str, MethodReflector* method)
	{
		methods.insert(std::make_pair(str, method));
	}

	template<typename T>
	void registerProperty(std::string str, int offset) {
		auto prop = new PropertyReflector(offset);
		
		if constexpr (std::is_base_of_v<TPointer, T> == true) {
			pointers.push_back(prop);
		}

		properties.insert(std::make_pair(str, prop));
	}

	inline bool isA(ObjectReflector* _parent) const {
		return reflation[N][_parent->N] & (EMatch::kIsAChildOf | EMatch::kIsASuperOf | EMatch::kSame);
	}

	inline bool isSame(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kSame);
	}

	inline bool isSuperOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kSuperOf);
	}

	inline bool isChildOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kChildOf);
	}

	inline bool isAChildOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kIsAChildOf | EMatch::kSame);
	}

	inline bool isASuperOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kIsASuperOf | EMatch::kSame);
	} 

	inline int getSize() {
		return size;
	}

	void markClassTree();
	void markIsATree(ObjectReflector* _);
	void implementsInterface(ObjectReflector* _interface);
	/*template<typename T>
	void registerProperty(std::string str, T method) {
		rep<T> func;
		methods.insert(std::make_pair(str, new reflectMethod(func.convert(method))));
	}*/
};

class TypeManager {
private:
	TypeManager() {}
	TypeManager(const TypeManager& ref) {}
	TypeManager& operator=(const TypeManager& ref) {}
	~TypeManager() {}
public:
	
	std::unordered_map<std::string, ObjectReflector*> objectReflections;
	std::unordered_map<ObjectReflector*, std::list<ObjectReflector*>> requirements;
	static TypeManager* get() {
		static TypeManager instance;
		return &instance;
	}
};
