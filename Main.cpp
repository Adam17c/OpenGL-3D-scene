#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include <iostream>
#include <vector>

#include <cmath>
#include "Shader.h"
//#include "load_obj.cpp"

using namespace std;

//glm::vec3 cameraPos = glm::vec3(-30.0f, 2.0f, 0.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -10.f);
//glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraAngle = 3.14/2;
float fov = 45.0f;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;



void processInput(GLFWwindow* window)
{
	float angleSpeed = 0.0001f;
	const float cameraSpeed = 0.005f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	//ruch kamer¹
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
		std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
		std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	}
	//rotacja kamer¹ w osi globalnej y
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraAngle -= angleSpeed;
		cameraFront = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraAngle += angleSpeed;
		cameraFront = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	}

}

char* Load_Shader(const char* fname)
{

	FILE* file = fopen(fname, "rb");
	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*)malloc(sizeof(char) * (fsize + 1));
	fread(buffer, 1, fsize, file);
	buffer[fsize] = 0;

	fclose(file);

	return buffer;
}

int Create_Vertex_Shader(const char* fname)
{
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char* vertexShaderSource = Load_Shader(fname);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(vertexShaderSource);
	return vertexShader;
}

int Create_Fragment_Shader(const char* fname)
{
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* vertexShaderSource = Load_Shader(fname);
	glShaderSource(fragmentShader, 1, &vertexShaderSource, NULL);
	glCompileShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(vertexShaderSource);
	return fragmentShader;
}

int Create_Program(int vertexShader, int fragmentShader)
{
	int success;
	char infoLog[512];

	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	return shaderProgram;
}

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Environment mapping", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);


	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	Shader shader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");

	int vertexShader = Create_Vertex_Shader("shaders/vertex_shader.vs");
	int fragmentShader = Create_Fragment_Shader("shaders/fragment_shader.fs");
	int Program = Create_Program(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glUseProgram(Program);

	shader.use();

	int modelloc = glGetUniformLocation(Program, "model");
	int viewloc = glGetUniformLocation(Program, "view");
	int projectionloc = glGetUniformLocation(Program, "projection");

	int cameraPosLoc = glGetUniformLocation(Program, "viewPos");

	// light struct values
	int lightConstantLoc = glGetUniformLocation(Program, "light.constant");
	glUniform1f(lightConstantLoc, 1.0f);
	shader.setFloat("light.constant", 1.0f);

	int lightLinearLoc = glGetUniformLocation(Program, "light.linear");
	glUniform1f(lightLinearLoc, 0.0014);
	shader.setFloat("light.linear", 0.0014);

	int lightQuadraticLoc = glGetUniformLocation(Program, "light.quadratic");
	glUniform1f(lightQuadraticLoc, 0.000007);
	shader.setFloat("light.quadratic", 0.000007);

	int lightPosLoc = glGetUniformLocation(Program, "light.position");
	glUniform3f(lightPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	shader.setVec3("light.position", cameraPos);

	int lightDirectionLoc = glGetUniformLocation(Program, "light.direction");
	glUniform3f(lightDirectionLoc, cameraFront.x, cameraFront.y, cameraFront.z);
	shader.setVec3("light.direction", cameraFront);

	int lightCutOffLoc = glGetUniformLocation(Program, "light.cutOff");
	glUniform1f(lightCutOffLoc, glm::cos(glm::radians(12.5f)));
	shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

	int lightOuterCutOffLoc = glGetUniformLocation(Program, "light.outerCutOff");
	glUniform1f(lightOuterCutOffLoc, glm::cos(glm::radians(17.5f)));
	shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

	int lightColorLoc = glGetUniformLocation(Program, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	int objectColorPos = glGetUniformLocation(Program, "objectColor");
	glUniform3f(objectColorPos, 1.0f, 0.5f, 0.31f);
	shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);


	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

	glUniformMatrix4fv(projectionloc, 1, GL_FALSE, glm::value_ptr(projection));
	shader.setMat4("projection", projection);


	//Object obj;


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// œwiat³o œwieci tam gdzie patrzymy
		// nie dzia³a
		glUniform3f(lightDirectionLoc, cameraFront.x, cameraFront.y, cameraFront.z);
		shader.setVec3("light.direction", cameraFront);
		glUniform3f(lightPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		shader.setVec3("light.position", cameraPos);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
		shader.setMat4("view", view);

		glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		shader.setVec3("viewPos", cameraPos);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(10, 10, 10));
		model = glm::rotate(model, 0.5f * (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(model));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);


		//glm::vec3 offset = glm::vec3(0.0f, 2 * sin((float)glfwGetTime()), -5.0f);
		//glUniform3f(lightColorLoc, offset.x, offset.y, offset.z);
		//model = glm::translate(glm::mat4(1.0f), offset);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, 45.0f, glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(model));
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}
