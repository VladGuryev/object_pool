#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
  T* Allocate(){
    if(!deallocated_pool.empty()){
      T* allocated_object = deallocated_pool.front();
      allocated_pool.insert(allocated_object);
      deallocated_pool.pop();
      return allocated_object;
    } else {
      T* some_object = new T;
      allocated_pool.insert(some_object);
      return some_object;
    }
  }
  T* TryAllocate(){
    if (deallocated_pool.empty())
      return nullptr;

    return this->Allocate();
  }

  void Deallocate(T* object){
      auto it = allocated_pool.find(object);

      if(it == allocated_pool.end())
        throw std::invalid_argument("");

      deallocated_pool.push(object);
      allocated_pool.erase(it);
  }

  ~ObjectPool(){
    while (!deallocated_pool.empty()) {
      delete deallocated_pool.front();
      deallocated_pool.pop();
    }
    for_each(allocated_pool.begin(),allocated_pool.end(),[](T* it){delete it;});
    allocated_pool.clear();
  }

private:
  queue<T*> deallocated_pool;
  set<T*> allocated_pool;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

//  string* t = new string("abc");
//  try {
//     pool.Deallocate(t);
//  } catch (invalid_argument& ia) {
//    cerr << ia.what() << "\n";
//  }

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
