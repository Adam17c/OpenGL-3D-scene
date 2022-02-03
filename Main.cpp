#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include <iostream>
#include <vector>

#include <cmath>
#include "Shader.h"
#include "Object.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -10.f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraAngle = 3.14/2;
float fov = 45.0f;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

unsigned int loadTexture(const char* path);

void processInput(GLFWwindow* window)
{
	float angleSpeed = 0.0008f;
	const float cameraSpeed = 0.005f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	//ruch kamer¹
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	Shader shader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");
	Shader textureShader("shaders/texture_shader.vs", "shaders/texture_shader.fs");

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// ³adownaie szeœcianów
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	//shader.use();
	textureShader.use();

	shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	shader.setInt("objectShine", 32);

	// light struct values
	shader.setFloat("light.constant", 1.0f);
	shader.setFloat("light.linear", 0.0014);
	shader.setFloat("light.quadratic", 0.00007);
	shader.setVec3("light.position", cameraPos.x, cameraPos.y, cameraPos.z 
	//	- 80.0
	);
	shader.setVec3("light.direction", cameraFront);

	shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
	shader.setVec3("light.color", 1.0f, 1.0f, 1.0f);
	shader.setFloat("light.ambient", 0.1f);
	shader.setFloat("light.specular", 0.5f);
	shader.setFloat("light.diffuse", 1.0f);

	// point light struct values
	shader.setVec3("pointLight.position", cameraPos.x, cameraPos.y, cameraPos.z + 100);
	// attenuation
	shader.setFloat("pointLight.constant", 1.0f);
	shader.setFloat("pointLight.linear", 0.0014);
	shader.setFloat("pointLight.quadratic", 0.000007);

	shader.setFloat("pointLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("pointLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	shader.setVec3("pointLight.color", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLight.ambient", 0.1f);
	shader.setFloat("pointLight.specular", 0.5f);
	shader.setFloat("pointLight.diffuse", 1.0f);

	// night factor
	shader.setFloat("nightFactor", 0.0f);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);


	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

	shader.setMat4("projection", projection);
	textureShader.setMat4("projection", projection);

	Object road;
	road.load_objn_mesh("objects/road.obj");
	road.Load_Texture("textures/bricks.jpg", 0);

	Object face;
	face.load_objn_mesh("objects/face.obj");
	face.Load_Texture("textures/face.png", 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// œwiat³o œwieci tam gdzie patrzymy
		shader.setVec3("light.direction", cameraFront);
		shader.setVec3("light.position", cameraPos);

		// night
		shader.setFloat("nightFactor", (sin((float)glfwGetTime() / 4) + 1 ) / 2);
		//shader.setFloat("nightFactor", 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", view);
		textureShader.setMat4("view", view);

		shader.setVec3("viewPos", cameraPos);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 10.0f));
		//model = glm::scale(model, glm::vec3(10, 10, 10));
		model = glm::rotate(model, 0.5f * (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		shader.setMat4("model", model);

		textureShader.setMat4("model", model);
		face.draw_mesh(0);

		//glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::rotate(model, 0.5f * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 1.72f, glm::vec3(1.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		textureShader.setMat4("model", model);
		road.draw_mesh(0);


		//model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
		//pin.draw_mesh(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}
