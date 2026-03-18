#include "PhysicsHeapAllocator.hpp"

#include <cstdlib>
#include <new>

PhysicsHeapAllocator::PhysicsHeapAllocator() : MemoryAllocator() {}
PhysicsHeapAllocator::~PhysicsHeapAllocator() {}

void *PhysicsHeapAllocator::allocate(size_t size)
{
    //void *block = std::aligned_alloc(16, size);
    void *block = _aligned_malloc(size, 16);
    if (!block) throw std::bad_alloc();
    return block;
}

void PhysicsHeapAllocator::release(void *block, size_t size) { _aligned_free(block); }