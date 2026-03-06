#ifndef POINTERRESOLVER_HPP
#define POINTERRESOLVER_HPP

/*
    this class is will be used to resolve pointer/API links between
    instances of different classes, such as AudioClip & AudioSource (for ex. to sync deleted AudioSource/AudioClip).
*/
class Engine;

class PointerResolver final
{
    friend class Engine;

    private:

};

#endif