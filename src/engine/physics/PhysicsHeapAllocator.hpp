#ifndef PHYSICSHEAPALLOCATIOR_HPP
#define PHYSICSHEAPALLOCATIOR_HPP

#include "../external/physics.hpp"

class PhysicsHeapAllocator final : public rp3d::MemoryAllocator
{
    public:
        PhysicsHeapAllocator();
        ~PhysicsHeapAllocator() override;

        void *allocate(size_t size);
        void release(void *block, size_t size);
};

#endif