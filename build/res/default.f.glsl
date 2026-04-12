#version 460 core
#extension GL_ARB_shader_storage_buffer_object : require

struct PointLightData
{
    vec3 position;
    float radius;
    vec3 color;
    uint flags;
};

uniform vec3 ambientLight;

uniform uint pointLightsCount;
layout(std430, binding = 0) readonly buffer pointLights
{
    PointLightData pointLightsData[];
};

in vec3 globalVertexPosition;
in vec2 texturePosition;
in vec3 normal;

out vec4 FragColor;

uniform vec4 color;

uniform bool hasTexture;
uniform sampler2D texture;

uniform bool fogEnabled;
uniform float fogStartDistance;
uniform float fogEndDistance;
uniform vec3 fogColor;

uniform vec3 cameraPosition;
uniform vec3 cameraRotation;

uniform vec3 cameraFront;
uniform vec3 cameraUp;
uniform vec3 cameraRight;

void main()
{
    vec4 vertcol = (hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color;

    vec3 light = vec3(0);
    for (uint i = 0; i < pointLightsCount; i++)
    {
        PointLightData data = pointLightsData[i];

        if (data.radius <= 0 || (data.flags & 1) == 0) continue;

        float dist = length(globalVertexPosition - data.pos);
        float lightintfactor = 1 - clamp(dist / data.radius, 0, 1);
        light += data.color * lightintfactor;
    }

    vertcol *= clamp(vec4(ambientLight + light, 1), vec4(0), vec4(1));

    float dist = length(globalVertexPosition - cameraPosition);
    float fog_int_factor = clamp((dist - fogStartDistance) / (fogEndDistance - fogStartDistance), 0, 1);

    FragColor = mix(vertcol, vec4(fogColor, 1), fog_int_factor);
}