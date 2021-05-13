#version 330 core

in vec3 TexCoords;

out vec4 gl_FragColor;

uniform samplerCube uSkybox;

void main()
{    
    gl_FragColor = texture(uSkybox, TexCoords);
}