#ifndef PHYSICSHEAPALLOCATIOR_HPP
#define PHYSICSHEAPALLOCATIOR_HPP

#include "../external/physics.hpp"

class Engine;

class PhysicsHeapAllocator final : public rp3d::MemoryAllocator
{
    friend class Engine;

    private:
        PhysicsHeapAllocator();
        ~PhysicsHeapAllocator() override;

    public:
        void *allocate(size_t size);
        void release(void *block, size_t size);
};

#endif