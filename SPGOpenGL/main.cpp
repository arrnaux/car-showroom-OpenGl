#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <stack>
#include <conio.h>
#include "objloader.hpp"

#define PI glm::pi<float>()

GLuint shader_programme, vao;
glm::mat4 projectionMatrix, viewMatrix, viewMatrixPerson, modelMatrix;
std::stack<glm::mat4> modelStack;
std::vector< glm::vec3 > verticesNormals;

GLuint vaoObj, vboObj;

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

float xv = 10, yv = 12, zv = 30; //originea sistemului de observare

glm::vec3 lightPos(1000, 0, 0);
glm::vec3 lightPos2(-1000, 0, 0);
glm::vec3 lightPosLego;

glm::vec3 viewPos(0, 0, 0);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float axisRotAngleLego = 0 ;
float axisRotAngleFirstCar = PI / 16.0f; // unghiul de rotatie in jurul propriei axe
float axisRotAngleSecondCar = PI / 16.0f;
float move = 0;
float direction = 0;
float radius = 2;
float scalingFactorCar = 2;
float scalingFactorLego = 0.05;
float obs_dir;
float obs_move;
size_t firstModelSize;

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

std::string textFileRead(char *fn)
{
	std::ifstream ifile(fn);
	std::string filetext;
	while (ifile.good()) {
		std::string line;
		std::getline(ifile, line);
		filetext.append(line + "\n");
	}
	return filetext;
}

void init()
{
	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	bool res = loadOBJ("obj/Dodge/CHALLENGER71.obj", vertices, uvs, normals);

	firstModelSize = vertices.size();

	res = loadOBJ("obj/lego.obj", vertices, uvs, normals);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 0);

	glewInit();

	verticesNormals = vertices;
	verticesNormals.insert(verticesNormals.end(), normals.begin(), normals.end());

	glGenBuffers(1, &vboObj);
	glBindBuffer(GL_ARRAY_BUFFER, vboObj);
	glBufferData(GL_ARRAY_BUFFER, verticesNormals.size() * sizeof(glm::vec3), &verticesNormals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoObj);
	glBindVertexArray(vaoObj);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices.size() * sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(firstModelSize * sizeof(glm::vec3)));

	std::string vstext = textFileRead("vertex.vert");
	std::string fstext = textFileRead("fragment.frag");
	const char* vertex_shader = vstext.c_str();
	const char* fragment_shader = fstext.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	printShaderInfoLog(fs);
	printShaderInfoLog(vs);
	printProgramInfoLog(shader_programme);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_programme);
	glBindVertexArray(vaoObj);

	GLuint lightPosLoc = glGetUniformLocation(shader_programme, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	GLuint lightPosition = glGetUniformLocation(shader_programme, "lightPos2");
	glUniform3fv(lightPosition, 1, glm::value_ptr(lightPos2));


	GLuint viewPosLoc = glGetUniformLocation(shader_programme, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

	//draw lego person
	modelMatrix = glm::mat4();
	modelMatrix *= glm::scale(glm::vec3(scalingFactorLego, scalingFactorLego, scalingFactorLego));
	modelMatrix *= glm::translate(glm::vec3(direction, 0, move));
	modelMatrix *= glm::rotate(axisRotAngleLego, glm::vec3(0, 1, 0));

	
	GLuint modelMatrixLoc = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrixPerson * modelMatrix));

	lightPosLego=glm::vec3(direction, 0, move);

	GLuint lightPosLocLego = glGetUniformLocation(shader_programme, "lightPos");
	glUniform3fv(lightPosLocLego, 1, glm::value_ptr(lightPosLego));

	//assign normals to lego
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	GLuint normalMatrixLoc = glGetUniformLocation(shader_programme, "normalMatrix");
	glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glDrawArrays(GL_TRIANGLES, firstModelSize, vertices.size() - firstModelSize);

	//draw first car
	modelMatrix = glm::mat4(); // matricea de modelare este matricea identitate
	modelMatrix *= glm::scale(glm::vec3(scalingFactorCar, scalingFactorCar, scalingFactorCar));
	modelMatrix *= glm::translate(glm::vec3(-5, 0, 0));
	modelMatrix *= glm::rotate(axisRotAngleFirstCar, glm::vec3(0, 1, 0));
	modelMatrix *= glm::rotate(PI / 2, glm::vec3(0, 0, 1));
	modelMatrix *= glm::rotate(PI, glm::vec3(1, 0, 0));
	modelMatrix *= glm::rotate(PI / 2, glm::vec3(0, 1, 0));
	modelMatrixLoc = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));



	

	//assign normals to first car
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	normalMatrixLoc = glGetUniformLocation(shader_programme, "normalMatrix");
	glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glDrawArrays(GL_TRIANGLES, 0, firstModelSize);

	//draw second car
	modelMatrix = glm::mat4(); // matricea de modelare este matricea identitate
	modelMatrix *= glm::scale(glm::vec3(scalingFactorCar, scalingFactorCar, scalingFactorCar));
	modelMatrix *= glm::translate(glm::vec3(5, 0, 0));
	modelMatrix *= glm::rotate(axisRotAngleSecondCar, glm::vec3(0, 1, 0));
	modelMatrix *= glm::rotate(-PI / 2, glm::vec3(0, 0, 1));
	modelMatrix *= glm::rotate(-PI, glm::vec3(1, 0, 0));
	modelMatrix *= glm::rotate(-PI / 2, glm::vec3(0, 1, 0));

	modelMatrixLoc = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));

	//assign normals to second car
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	normalMatrixLoc = glGetUniformLocation(shader_programme, "normalMatrix");
	glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glDrawArrays(GL_TRIANGLES, 0, firstModelSize);
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	projectionMatrix = glm::perspective(PI / 4, (float)w / h, 0.1f, 1000.0f);
	/*
	viewMatrix este matricea transformarii de observare. Parametrii functiei
	lookAt sunt trei vectori ce reprezinta, in ordine:
	- pozitia observatorului
	- punctul catre care priveste observatorul
	- directia dupa care este orientat observatorul
	*/
	viewMatrix = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	viewMatrixPerson = glm::lookAt(glm::vec3(xv, yv, zv), glm::vec3(obs_dir, 0, obs_move), glm::vec3(0, 1, 0));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		axisRotAngleFirstCar += 0.1f;
		axisRotAngleSecondCar -= 0.1f;
		if (axisRotAngleFirstCar > 2 * PI)
			axisRotAngleFirstCar = 0;
		if (axisRotAngleSecondCar < 0)
			axisRotAngleSecondCar = 2 * PI;
		break;
	case 's':
		axisRotAngleFirstCar -= 0.1f;
		axisRotAngleSecondCar += 0.1f;
		if (axisRotAngleFirstCar < 0)
			axisRotAngleFirstCar = 2 * PI;
		if (axisRotAngleSecondCar > 2 * PI)
			axisRotAngleSecondCar = 0;
		break;
	case '+':
		scalingFactorCar += 0.3f;
		scalingFactorLego += 0.01f;
		
		break;
	case '-':
		scalingFactorCar -= 0.3f;
		scalingFactorLego -= 0.01f;
		break;
	case 'c':
		axisRotAngleLego += PI / 2;
	case 'i':
		move -= 5.0f;
		//obs_move -= 1.0f;
		break;
	case 'k':
		move += 5.0f;
		//obs_move += 1.0f;
		break;
	case 'j':
		direction -= 5.0f;
		break;
	case 'l':
		direction += 5.0f;
		break;

	};
	glutPostRedisplay(); //redraw the window
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(700, 700);
	glutCreateWindow("SPG thuglife");
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
