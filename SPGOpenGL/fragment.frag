#version 400

out vec4 fragColor;
in vec3 normal;
in vec3 pos;
in vec3 posLego;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightPosLego;
uniform vec3 lightPos2;

vec3 lighting(vec3 objectColor, vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos,
	vec3 ambient, vec3 lightColor, vec3 specular, float specPower)
{
	vec3 L = normalize(lightPos - pos);
	vec3 V = normalize(viewPos - pos);
	vec3 N = normalize(normal);
	vec3 R = normalize(reflect(-L, N));

	float diffCoef = max(dot(L, N), 0.0);
	float specCoef = pow(dot(R, V), specPower);

	vec3 ambientColor = ambient * lightColor;
	vec3 diffuseColor = diffCoef * lightColor;
	vec3 specularColor = specCoef * specular * lightColor;
	vec3 col = (ambientColor + diffuseColor + specularColor) * objectColor;

	return clamp(col, 0, 1);
}

void main()
{
	vec3 objectColor = vec3(1.0, 1.0, 1.0);
	vec3 lightColor = vec3(0.0, 1.0, 1.0);
	vec3 ambient = vec3(0.1);
	vec3 specular = vec3(0.8);
	vec3 color;
	float specPower = 32;
	//if (vec3(gl_LightSource[0].position) != vec3(0.0, 0.0, 0.0))
	//	light += light0();
	//if (vec3(gl_LightSource[1].position) != vec3(0.0, 0.0, 0.0))
	//	light += light1();
	//if(lightPosLego != vec3(0.0,0.0,0.0))
	//	color += lighting(objectColorLego, posLego, normal, lightPosLego, viewPos, ambientLego, lightColorLego, specularLego, specPowerLego);
	/*if (lightPos != vec3(0.0, 0.0, 0.0))*/

	//color += lighting(objectColor, posLego, normal, lightPos, viewPos, ambient, lightColor, specular, specPower);

	color += lighting(objectColor, pos, normal, lightPos, viewPos, ambient, lightColor, specular, specPower);
	//vec3 pos2 = vec3(-5, 5, 0);

	color += lighting(objectColor, posLego, normal, lightPos2, viewPos, ambient, lightColor, specular, specPower);
	
	fragColor = vec4(color, 1.0);
}
