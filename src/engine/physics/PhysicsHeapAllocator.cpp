#include "PhysicsHeapAllocator.hpp"

#include <cstdlib>

PhysicsHeapAllocator::PhysicsHeapAllocator() : MemoryAllocator() {}
PhysicsHeapAllocator::~PhysicsHeapAllocator() {}

void *PhysicsHeapAllocator::allocate(size_t size) { return malloc(size); }
void PhysicsHeapAllocator::release(void *block, size_t size) { free(block); }