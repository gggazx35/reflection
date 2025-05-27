//#include "TypeManager.h"
#include "ObjectReflector.h"

unsigned char ObjectReflector::reflation[20][20] = {0};
int ObjectReflector::refl_index = 0;
//class TypeManager;
//
//ObjectReflector::ObjectReflector(void (*init)(ObjectReflector*))
//{
//	
//
//	 init(this);
//	 //registerObject(name, this);
//	 TypeManager::get()->objectReflections.emplace(std::make_pair(name, this));
//	 N = refl_index++;
//	 reflation[N][N] = (EMatch::kSame);
//
//	 super = nullptr;
//
//	 if (TypeManager::get()->requirements.count(this)) {
//		 auto val = TypeManager::get()->requirements.at(this);
//		 markClassTree(val);
//	 }
//}

//ObjectReflector::ObjectReflector(void (*init)(ObjectReflector*), ObjectReflector* _super) {
//	if(_super) TypeManager::get()->requirements[_super].push_back(this);
//	init(this);
//	super = _super;
//	N = refl_index++;
//	
//	if (TypeManager::get()->requirements.count(this)) {
//		std::cout << name << '\n';
//		auto val = TypeManager::get()->requirements.at(this);
//		markClassTree();
//	}
//
//	reflation[N][N] = EMatch::kSame;
//	TypeManager::get()->objectReflections.emplace(std::make_pair(name, this));
//	
//}

void ObjectReflector::markClassTree()
{
	auto nval = TypeManager::get()->requirements.at(this);
	for (auto val : nval) {
		std::cout << val->name << " : " << name << '\n';
		val->methods.insert(methods.begin(), methods.end());

		//val->methods = _super->methods;
		val->properties.insert(properties.begin(), properties.end());
		for(auto k : pointers) val->pointers.push_back(k);
		//pointers.assign(_super->pointers.begin(), _super->pointers.end());
		reflation[val->N][N] = (EMatch::kChildOf | EMatch::kIsAChildOf);

		reflation[N][val->N] = (EMatch::kSuperOf | EMatch::kIsASuperOf);

		val->markIsATree(val);

		//auto curr = super->super;
		if (TypeManager::get()->requirements.count(val)) {
			val->markClassTree();
		}
	}
}

void ObjectReflector::markIsATree(ObjectReflector* _)
{
	for (auto curr : interfaces) { // curr = super
		//auto curr = item;
		//while (curr != nullptr) {
		if(reflation[_->N][curr->N] == 0) reflation[_->N][curr->N] = EMatch::kIsAChildOf;
		if(reflation[curr->N][_->N] == 0) reflation[curr->N][_->N] = EMatch::kIsASuperOf;

			std::cout << _->name << " is child of " << curr->name << '\n';
		//}
		curr->markIsATree(_);
		//}
	}
}

void ObjectReflector::implementsInterface(ObjectReflector* _interface)
{
}

