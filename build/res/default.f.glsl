#version 330 core

#define MAX_POINT_LIGHTS 8

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

uniform vec3 ambientColor;
uniform bool hasDirectionalLight;
uniform vec3 directionalLightDir;
uniform vec3 directionalLightColor;
uniform float directionalLightIntensity;

uniform int pointLightCount;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];
uniform float pointLightIntensity[MAX_POINT_LIGHTS];
uniform float pointLightRange[MAX_POINT_LIGHTS];

vec3 calcPointLights(vec3 n, vec3 worldPos)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < pointLightCount; ++i)
    {
        vec3 toLight = pointLightPos[i] - worldPos;
        float dist = length(toLight);
        if (dist > pointLightRange[i]) continue;

        vec3 l = normalize(toLight);
        float attenuation = 1.0 - smoothstep(pointLightRange[i] * 0.6, pointLightRange[i], dist);
        result += max(dot(n, l), 0.0) * pointLightColor[i] * pointLightIntensity[i] * attenuation;
    }

    return result;
}

void main()
{
    vec4 albedo = (hasTexture ? texture2D(texture, texturePosition) : vec4(1.0)) * color;
    vec3 n = normalize(normal);

    vec3 lit = ambientColor * albedo.rgb;

    if (hasDirectionalLight)
    {
        vec3 l = normalize(-directionalLightDir);
        lit += max(dot(n, l), 0.0) * directionalLightColor * directionalLightIntensity * albedo.rgb;
    }

    lit += calcPointLights(n, globalVertexPosition) * albedo.rgb;
    vec4 vertcol = vec4(lit, albedo.a);

    float dist = length(globalVertexPosition - cameraPosition);
    float fog_int_factor = min(1.0, max(0.0, (dist - fogStartDistance) / (fogEndDistance - fogStartDistance)));

    FragColor = mix(vertcol, vec4(fogColor, vertcol.a), fog_int_factor);
}
