#version 330 core

in vec3 vSkyPosition;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = vSkyPosition;
    gl_Position = projection * view * vec4(vSkyPosition, 1.0);
}  