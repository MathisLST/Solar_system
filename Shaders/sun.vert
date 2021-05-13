#version 130
precision mediump float;

attribute vec3 vPosition; 
attribute vec3 vNormal;
attribute vec2 vUV;

uniform mat4 uVP;
uniform mat4 uPL;
uniform mat4 uPLinv;

varying vec2 vary_uv;
varying vec3 Normal;
varying vec3 FragPosition;


void main()
{
	gl_Position = uVP* uPL * vec4(vPosition, 1.0);
	FragPosition = vec3(uPL * vec4(vPosition, 1.0));
	Normal = mat3(transpose(uPLinv)) * vNormal;
	vary_uv = vUV;
}