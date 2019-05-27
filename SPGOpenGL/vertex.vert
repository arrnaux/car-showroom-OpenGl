#version 400

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vPosLego;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

out vec3 normal;
out vec3 pos;
out vec3 posLego;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vPos, 1.0);
	normal = vNormal;
	normal = vec3(normalMatrix * vec4(normal, 1));
	posLego = vPosLego;
	pos = vPos;
}