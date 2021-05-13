#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used


varying vec3 Normal;
varying vec3 FragPosition;
varying vec2 vary_uv;

uniform sampler2D skin;


void main()
{
	vec3 result = vec3(texture(skin, vary_uv));
	gl_FragColor = vec4(result, 1.0);
}