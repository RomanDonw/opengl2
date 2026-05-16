#include "EditorGizmos.hpp"

#include "EditorCamera.hpp"
#include "EditorState.hpp"

#include "engine/Scene.hpp"
#include "engine/external/imgui.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/FreeplayCamera/FreeplayCamera.hpp"
#include "engine/objects/Light/DirectionalLight.hpp"
#include "engine/objects/Light/PointLight.hpp"

#include <cmath>
#include <cstdio>
#include <vector>

namespace
{
    struct LineSeg
    {
        glm::vec3 a;
        glm::vec3 b;
        ImU32 color;
        float thickness = 1.5f;
    };

    static bool worldToScreen(const glm::mat4 &vp, const ImVec2 &vpMin, const ImVec2 &vpSize, const glm::vec3 &world, ImVec2 &out)
    {
        const glm::vec4 clip = vp * glm::vec4(world, 1.0f);
        if (clip.w <= 0.0001f) return false;

        const glm::vec3 ndc = glm::vec3(clip) / clip.w;
        if (ndc.x < -1.05f || ndc.x > 1.05f || ndc.y < -1.05f || ndc.y > 1.05f || ndc.z < -1.05f || ndc.z > 1.05f)
            return false;

        out.x = vpMin.x + (ndc.x * 0.5f + 0.5f) * vpSize.x;
        out.y = vpMin.y + (1.0f - (ndc.y * 0.5f + 0.5f)) * vpSize.y;
        return true;
    }

    static void addLine(std::vector<LineSeg> &lines, const glm::vec3 &a, const glm::vec3 &b, ImU32 color, float thickness = 1.5f)
    {
        lines.push_back({a, b, color, thickness});
    }

    static void addCircleXZ(std::vector<LineSeg> &lines, const glm::vec3 &center, float radius, ImU32 color, int segments = 24)
    {
        if (radius <= 0.001f) return;
        glm::vec3 prev = center + glm::vec3(radius, 0, 0);
        for (int i = 1; i <= segments; ++i)
        {
            const float t = (static_cast<float>(i) / static_cast<float>(segments)) * 6.2831853f;
            const glm::vec3 p = center + glm::vec3(std::cos(t) * radius, 0, std::sin(t) * radius);
            addLine(lines, prev, p, color, 1.0f);
            prev = p;
        }
    }

    static void addCircleXY(std::vector<LineSeg> &lines, const glm::vec3 &center, float radius, ImU32 color, int segments = 16)
    {
        if (radius <= 0.001f) return;
        glm::vec3 prev = center + glm::vec3(radius, 0, 0);
        for (int i = 1; i <= segments; ++i)
        {
            const float t = (static_cast<float>(i) / static_cast<float>(segments)) * 6.2831853f;
            const glm::vec3 p = center + glm::vec3(std::cos(t) * radius, std::sin(t) * radius, 0);
            addLine(lines, prev, p, color, 1.0f);
            prev = p;
        }
    }

    static void drawLines(ImDrawList *dl, const glm::mat4 &vp, const ImVec2 &vpMin, const ImVec2 &vpSize, const std::vector<LineSeg> &lines)
    {
        for (const LineSeg &seg : lines)
        {
            ImVec2 sa, sb;
            if (!worldToScreen(vp, vpMin, vpSize, seg.a, sa)) continue;
            if (!worldToScreen(vp, vpMin, vpSize, seg.b, sb)) continue;
            dl->AddLine(sa, sb, seg.color, seg.thickness);
        }
    }

    static void drawCameraIcon(std::vector<LineSeg> &lines, Camera *cam, bool active, bool selected)
    {
        if (!cam || cam->displayName == "__EditorCamera__") return;

        const Transform gt = cam->GetGlobalTransform();
        const glm::vec3 p = gt.GetPosition();
        const glm::vec3 f = glm::normalize(gt.GetFront());
        const glm::vec3 r = glm::normalize(gt.GetRight());
        const glm::vec3 u = glm::normalize(gt.GetUp());

        const float body = selected ? 0.35f : 0.28f;
        const float arrowLen = selected ? 1.6f : 1.2f;
        const ImU32 col = active ? IM_COL32(80, 255, 140, 255) : IM_COL32(90, 210, 255, 255);
        const float thick = selected ? 2.5f : 1.8f;

        const glm::vec3 baseC = p + f * body;
        const glm::vec3 tl = baseC + u * body * 0.55f - r * body * 0.7f;
        const glm::vec3 tr = baseC + u * body * 0.55f + r * body * 0.7f;
        const glm::vec3 bl = baseC - u * body * 0.55f - r * body * 0.7f;
        const glm::vec3 br = baseC - u * body * 0.55f + r * body * 0.7f;

        addLine(lines, p, baseC, col, thick);
        addLine(lines, p, tl, col, thick);
        addLine(lines, p, tr, col, thick);
        addLine(lines, p, bl, col, thick);
        addLine(lines, p, br, col, thick);
        addLine(lines, tl, tr, col, thick);
        addLine(lines, tr, br, col, thick);
        addLine(lines, br, bl, col, thick);
        addLine(lines, bl, tl, col, thick);

        const glm::vec3 tip = p + f * arrowLen;
        addLine(lines, p, tip, col, thick + 0.5f);
        const glm::vec3 head = tip - f * 0.18f;
        addLine(lines, tip, head + u * 0.1f, col, thick);
        addLine(lines, tip, head - u * 0.1f, col, thick);
        addLine(lines, tip, head + r * 0.1f, col, thick);
        addLine(lines, tip, head - r * 0.1f, col, thick);

        const float halfTan = std::tan(cam->FOV * 0.5f);
        const float nearD = cam->nearDistance * 2.0f;
        const float farD = glm::clamp(cam->nearDistance * 8.0f, 0.5f, 3.0f);
        const float nH = nearD * halfTan;
        const float fH = farD * halfTan;

        const glm::vec3 nc = p + f * nearD;
        const glm::vec3 fc = p + f * farD;
        const glm::vec3 ntl = nc + u * nH - r * nH;
        const glm::vec3 ntr = nc + u * nH + r * nH;
        const glm::vec3 nbl = nc - u * nH - r * nH;
        const glm::vec3 nbr = nc - u * nH + r * nH;
        const glm::vec3 ftl = fc + u * fH - r * fH;
        const glm::vec3 ftr = fc + u * fH + r * fH;
        const glm::vec3 fbl = fc - u * fH - r * fH;
        const glm::vec3 fbr = fc - u * fH + r * fH;

        const ImU32 frustumCol = IM_COL32(90, 210, 255, 120);
        addLine(lines, ntl, ntr, frustumCol, 1.0f);
        addLine(lines, ntr, nbr, frustumCol, 1.0f);
        addLine(lines, nbr, nbl, frustumCol, 1.0f);
        addLine(lines, nbl, ntl, frustumCol, 1.0f);
        addLine(lines, ftl, ftr, frustumCol, 1.0f);
        addLine(lines, ftr, fbr, frustumCol, 1.0f);
        addLine(lines, fbr, fbl, frustumCol, 1.0f);
        addLine(lines, fbl, ftl, frustumCol, 1.0f);
        addLine(lines, ntl, ftl, frustumCol, 1.0f);
        addLine(lines, ntr, ftr, frustumCol, 1.0f);
        addLine(lines, nbl, fbl, frustumCol, 1.0f);
        addLine(lines, nbr, fbr, frustumCol, 1.0f);
    }

    static void drawPointLightIcon(std::vector<LineSeg> &lines, PointLight *pl, bool selected)
    {
        if (!pl || !pl->enabled) return;

        const glm::vec3 p = pl->GetGlobalTransform().GetPosition();
        const ImU32 col = IM_COL32(
            static_cast<int>(glm::clamp(pl->color.r * pl->intensity, 0.0f, 1.0f) * 255),
            static_cast<int>(glm::clamp(pl->color.g * pl->intensity, 0.0f, 1.0f) * 255),
            static_cast<int>(glm::clamp(pl->color.b * pl->intensity, 0.0f, 1.0f) * 255),
            255);
        const float thick = selected ? 2.4f : 1.6f;

        const float r = 0.18f;
        addLine(lines, p + glm::vec3(-r, 0, 0), p + glm::vec3(r, 0, 0), col, thick);
        addLine(lines, p + glm::vec3(0, -r, 0), p + glm::vec3(0, r, 0), col, thick);
        addLine(lines, p + glm::vec3(0, 0, -r), p + glm::vec3(0, 0, r), col, thick);

        addCircleXZ(lines, p, pl->range, IM_COL32(255, 220, 120, 140), 32);
        addCircleXY(lines, p, pl->range * 0.35f, IM_COL32(255, 220, 120, 90), 16);
    }

    static void drawDirectionalLightIcon(std::vector<LineSeg> &lines, DirectionalLight *dl, bool selected)
    {
        if (!dl || !dl->enabled) return;

        const Transform gt = dl->GetGlobalTransform();
        const glm::vec3 p = gt.GetPosition();
        const glm::vec3 dir = glm::normalize(gt.GetFront());
        const glm::vec3 r = glm::normalize(gt.GetRight());
        const glm::vec3 u = glm::normalize(gt.GetUp());

        const ImU32 col = IM_COL32(
            static_cast<int>(glm::clamp(dl->color.r * dl->intensity, 0.0f, 1.0f) * 255),
            static_cast<int>(glm::clamp(dl->color.g * dl->intensity, 0.0f, 1.0f) * 255),
            static_cast<int>(glm::clamp(dl->color.b * dl->intensity, 0.0f, 1.0f) * 255),
            255);
        const float thick = selected ? 2.6f : 1.8f;
        const float discR = selected ? 0.45f : 0.35f;
        const float arrowLen = selected ? 2.2f : 1.8f;

        glm::vec3 prev = p + r * discR;
        for (int i = 1; i <= 12; ++i)
        {
            const float t = (static_cast<float>(i) / 12.0f) * 6.2831853f;
            const glm::vec3 pt = p + r * std::cos(t) * discR + u * std::sin(t) * discR;
            addLine(lines, prev, pt, col, thick);
            prev = pt;
        }

        const glm::vec3 tip = p + dir * arrowLen;
        addLine(lines, p, tip, col, thick + 0.5f);
        const glm::vec3 head = tip - dir * 0.22f;
        addLine(lines, tip, head + u * 0.14f, col, thick);
        addLine(lines, tip, head - u * 0.14f, col, thick);
        addLine(lines, tip, head + r * 0.14f, col, thick);
        addLine(lines, tip, head - r * 0.14f, col, thick);

        for (int i = -2; i <= 2; ++i)
        {
            if (i == 0) continue;
            const glm::vec3 off = (r * static_cast<float>(i) * 0.12f) + (u * static_cast<float>(i) * 0.08f);
            addLine(lines, p + off, p + off + dir * (arrowLen * 0.65f), IM_COL32(255, 240, 180, 160), 1.0f);
        }
    }
}

void EditorGizmos::DrawViewportOverlay(EditorState &state, const ImVec2 &imageMin, const ImVec2 &imageSize)
{
    if (!state.scene || imageSize.x < 8 || imageSize.y < 8) return;

    const glm::uvec2 vpSize(static_cast<unsigned>(imageSize.x), static_cast<unsigned>(imageSize.y));
    const glm::mat4 view = state.flyCamera.GetViewMatrix();
    const glm::mat4 proj = state.flyCamera.GetProjectionMatrix(vpSize);
    const glm::mat4 vp = proj * view;

    Camera *activeCam = state.scene->GetCurrentCamera();
    if (state.isPlaying && state.freeplayViewportPreview)
        activeCam = state.FindFreeplayCamera();

    std::vector<LineSeg> lines;
    state.scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj == state.editorCameraObject) return true;

        const bool selected = state.selected == obj;
        if (Camera *cam = dynamic_cast<Camera *>(obj))
        {
            const bool active = cam == activeCam;
            drawCameraIcon(lines, cam, active, selected);
        }
        else if (PointLight *pl = dynamic_cast<PointLight *>(obj))
            drawPointLightIcon(lines, pl, selected);
        else if (DirectionalLight *dl = dynamic_cast<DirectionalLight *>(obj))
            drawDirectionalLightIcon(lines, dl, selected);

        return true;
    });

    ImDrawList *dl = ImGUI::GetWindowDrawList();
    drawLines(dl, vp, imageMin, imageSize, lines);

    ImVec2 labelPos;
    state.scene->ForEachAllObjects([&](GameObject *obj) -> bool
    {
        if (obj == state.editorCameraObject) return true;
        if (!dynamic_cast<Camera *>(obj) && !dynamic_cast<PointLight *>(obj) && !dynamic_cast<DirectionalLight *>(obj))
            return true;

        const glm::vec3 p = obj->GetGlobalTransform().GetPosition();
        if (!worldToScreen(vp, imageMin, imageSize, p, labelPos)) return true;

        const char *kind = dynamic_cast<Camera *>(obj) ? "CAM" : dynamic_cast<PointLight *>(obj) ? "PT" : "DIR";
        char buf[128];
        std::snprintf(buf, sizeof(buf), "%s %s", kind, obj->displayName.c_str());
        dl->AddText(ImVec2(labelPos.x + 6, labelPos.y - 14), IM_COL32(230, 230, 240, 230), buf);
        return true;
    });
}
