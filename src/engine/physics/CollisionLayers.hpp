#ifndef COLLISIONLAYERS_HPP
#define COLLISIONLAYERS_HPP

namespace CollisionLayer
{
    constexpr unsigned short Default = 0x0001;
    constexpr unsigned short Player = 0x0002;
    constexpr unsigned short World = 0x0004;
    constexpr unsigned short Interactable = 0x0008;
    constexpr unsigned short Projectile = 0x0010;
    constexpr unsigned short Trigger = 0x0020;

    constexpr unsigned short All = 0xFFFF;
}

#endif
