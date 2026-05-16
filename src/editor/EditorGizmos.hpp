#ifndef EDITORGIZMOS_HPP
#define EDITORGIZMOS_HPP

#include "engine/external/glm.hpp"

struct ImVec2;

class EditorState;

class EditorGizmos final
{
    public:
        static void DrawViewportOverlay(EditorState &state, const ImVec2 &imageMin, const ImVec2 &imageSize);
};

#endif
