#include <cassert>
#include <ctime>
#include <iostream>
#include <vector>

#include "MemoryPool.hpp"
#include "StackAlloc.hpp"


#define ELEMS 100000
#define REPS 50

using namespace Diana;

int main() {
	clock_t start;

	StackAlloc<int, std::allocator<int>> stackDefault;
	start = clock();
	for (int i = 0; i < REPS; ++i) {
		assert(stackDefault.empty());
		for (int j = 0; j < ELEMS; ++j)
			stackDefault.push(j);
		for (int j = 0; j < ELEMS; ++j)
			stackDefault.pop();
	}
	std::cout << "Default Allocator Time: ";
	std::cout << (((double) clock() - start) / CLOCKS_PER_SEC) << std::endl;

	// 线程池
	StackAlloc<int, MemoryPool<int>> stackPool;
	start = clock();
	for (int i = 0; i < REPS; ++i) {
		assert(stackPool.empty());
		for (int j = 0; j < ELEMS; ++j)
			stackPool.push(j);
		for (int j = 0; j < ELEMS; ++j)
			stackPool.pop();
	}
	std::cout << "MemoryPool Allocator Time: ";
	std::cout << (((double) clock() - start) / CLOCKS_PER_SEC) << std::endl;

	// Vector
	std::vector<int> vecDefault;
	start = clock();
	for (int i = 0; i < REPS; ++i) {
		assert(vecDefault.empty());
		for (int j = 0; j < ELEMS; ++j)
			vecDefault.push_back(j);
		for (int j = 0; j < ELEMS; ++j)
			vecDefault.pop_back();
	}
	std::cout << "Vector Default Allocator Time: ";
	std::cout << (((double) clock() - start) / CLOCKS_PER_SEC) << std::endl;

	return 0;
}
