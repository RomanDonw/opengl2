#version 460 core

struct PointLightData
{
    vec3 position;
    float radius;
    vec3 color;
    uint flags;
};

uniform vec3 ambientLight;

uniform uint pointLightsCount;
layout(std430, binding = 0) buffer pointLights
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

    vertcol *= vec4(ambientLight, 1);

    float dist = length(globalVertexPosition - cameraPosition);
    float fog_int_factor = min(1, max(0, (dist - fogStartDistance) / (fogEndDistance - fogStartDistance)));

    FragColor = mix(vertcol, vec4(fogColor, 1), fog_int_factor);
}