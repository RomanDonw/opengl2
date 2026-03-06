#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexturePosition;

out vec3 globalVertexPosition;
out vec2 texturePosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 globvpos4 = model * vec4(vertexPosition, 1.0);

    globalVertexPosition = vec3(globvpos4.x, globvpos4.y, globvpos4.z);
    texturePosition = vertexTexturePosition;

    gl_Position = projection * view * globvpos4;
}