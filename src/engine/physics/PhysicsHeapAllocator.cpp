#include "PhysicsHeapAllocator.hpp"

#include <cstdlib>
#include <new>

// === PRIVATE ===

PhysicsHeapAllocator::PhysicsHeapAllocator() : MemoryAllocator() {}
PhysicsHeapAllocator::~PhysicsHeapAllocator() {}

// === PUBLIC ===

void *PhysicsHeapAllocator::allocate(size_t size)
{
    //void *block = std::aligned_alloc(16, size);
    #ifdef _WIN32
        void *block = _aligned_malloc(size, 16);
    #else
        void *block = std::aligned_alloc(16, size);
    #endif

    if (!block) throw std::bad_alloc();
    return block;
}

void PhysicsHeapAllocator::release(void *block, size_t size)
{
    #ifdef _WIN32
        _aligned_free(block);
    #else
        std::free(block);
    #endif
}