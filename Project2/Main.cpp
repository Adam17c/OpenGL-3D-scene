#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>
#include "Shader.h"
#include "Object.cpp"
#include <map>

using namespace std;

Shader createMainShader(const char* vertexShader, const char* fragmentShader);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -10.f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraAngle = (float)M_PI / 2;
float fov = 45.0f;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

enum ButtonDown
{
	one,
	two,
	three,
	four,
	K,
	L,
	R,
	none
};
ButtonDown buttonClicked = none;

enum CameraOption
{
	watchingScene,
	watchingMovingObject,
	followingMovingObject,
	freeCam,
};
CameraOption cameraOption = freeCam;

enum ShadingOption
{
	phong,
	gouraud
};
ShadingOption shadingOption = phong;

Shader shader, phongShader, gouraudShader;
bool switchShader;

bool switchLighting;
bool blinnLighting = false;

bool movingReflector = false;
glm::vec3 reflectorDirection = glm::vec3(0.0f, 0.0f, 1.0f);
bool reflectorDirectionChanged = false;

void processInput(GLFWwindow* window)
{
		float angleSpeed = 0.0008f;
		const float cameraSpeed = 0.005f;

		// camera/reflector movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (movingReflector)
			{
				reflectorDirection.y += angleSpeed;
				reflectorDirectionChanged = true;
				return;
			}
			if(cameraOption == freeCam) cameraPos += cameraSpeed * cameraFront;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (movingReflector)
			{
				reflectorDirection.y -= angleSpeed;
				reflectorDirectionChanged = true;
				return;
			}
			if (cameraOption == freeCam) cameraPos -= cameraSpeed * cameraFront;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if (movingReflector)
			{
				reflectorDirection.x += angleSpeed;
				reflectorDirectionChanged = true;
				return;
			}
			if (cameraOption == freeCam) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if (movingReflector)
			{
				reflectorDirection.x -= angleSpeed;
				reflectorDirectionChanged = true;
				return;
			}
			if (cameraOption == freeCam) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		//rotacja kamer¹ w osi y
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			cameraAngle -= angleSpeed;
			if (cameraOption == freeCam) cameraFront = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			cameraAngle += angleSpeed;
			if (cameraOption == freeCam) cameraFront = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
		}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// switching camera options
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		buttonClicked = one;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && buttonClicked == one)
	{
		buttonClicked = none;
		cameraOption = watchingScene;
		cameraPos = glm::vec3(-35.0f, 3.0f, 11.0f);
		cameraFront = glm::vec3(0.9f, -0.2f, -0.5f);
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		buttonClicked = two;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE && buttonClicked == two)
	{
		buttonClicked = none;
		cameraOption = watchingMovingObject;
		cameraPos = glm::vec3(0.0f, 0.0f, -10.f);
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		buttonClicked = three;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE && buttonClicked == three)
	{
		buttonClicked = none;
		cameraOption = followingMovingObject;
		cameraPos.y = 0.0f;
		cameraPos.z = -2.2f;
		cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		buttonClicked = four;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE && buttonClicked == four)
	{
		buttonClicked = none;
		cameraOption = freeCam;
		cameraPos.y = cameraFront.y = 0;
	}

	// switch shading
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		buttonClicked = K;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE && buttonClicked == K)
	{
		buttonClicked = none;
		switchShader = true;
		if (shadingOption == phong)
		{
			shadingOption = gouraud;
		}
		else
		{
			shadingOption = phong;
		}
	}

	// switch lighting
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		buttonClicked = L;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && buttonClicked == L)
	{
		buttonClicked = none;
		switchLighting = true;
		blinnLighting = !blinnLighting;
	}

	// enable moving reflector
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		buttonClicked = R;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && buttonClicked == R)
	{
		buttonClicked = none;
		movingReflector = !movingReflector;
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

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float))); // v
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // vn
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // uv
	glEnableVertexAttribArray(1);

	Shader liightSourceShader("shaders/light_source_shader.vs", "shaders/light_source_shader.fs");
	phongShader = createMainShader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");
	gouraudShader = createMainShader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");
	shader = createMainShader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	shader.setMat4("projection", projection);
	liightSourceShader.use();
	liightSourceShader.setMat4("projection", projection);

	Object pin;
	pin.LoadObjectMesh("objects/pin.obj");
	pin.LoadTexture("textures/pin.jpeg", 0);

	Object floor;
	floor.LoadObjectMesh("objects/plane.obj");
	floor.LoadTexture("textures/floor.jpg", 0);

	Object ball;
	ball.LoadObjectMesh("objects/ball.obj");
	ball.LoadTexture("textures/red.jpg", 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		if (switchShader)
		{
			liightSourceShader.use();
			if (shadingOption == phong)
				shader = createMainShader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");
			else
				shader = createMainShader("shaders/gouraud_shader.vs", "shaders/gouraud_shader.fs");
			shader.use();
			shader.setMat4("projection", projection);
			switchShader = false;
		}

		if (switchLighting)
		{
			switchLighting = false;
			shader.setBool("blinn", blinnLighting);
		}

		if (reflectorDirectionChanged)
		{
			reflectorDirectionChanged = false;
			shader.setVec3("light.direction", reflectorDirection);
		}

		// night factor
		shader.use();
		shader.setFloat("nightFactor", (sin((float)glfwGetTime() / 4) + 1) / 2);

		// camera pos
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", cameraPos);

		// rysowanie obiektów
		shader.use();
		// pins
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		shader.setMat4("model", model);
		pin.DrawMesh(0);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f));
		shader.setMat4("model", model);
		pin.DrawMesh(0);

		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -2.0f));
		shader.setMat4("model", model);
		pin.DrawMesh(0);

		// floor
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 4.0f));
		shader.setMat4("model", model);
		floor.DrawMesh(0);

		// static ball
		shader.setBool("drawSmoothObject", true);
		shader.setVec3("smoothObjectColor", glm::vec3(0.6, 0.6, 1.0));
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, -1.0f, 3.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		ball.DrawMesh(0);

		float vibrations = 0.02f * sin((float)glfwGetTime() * 30);
		// moving ball
		shader.setVec3("smoothObjectColor", glm::vec3(1.0, 0.5, 0.5));
		float movingBallX = 10.0f * sin((float)glfwGetTime() / 4) - 5.0f;
		model = glm::translate(glm::mat4(1.0f), glm::vec3(movingBallX, -1.0f + vibrations, -2.2f));
		model = glm::rotate(model, sin((float)glfwGetTime() / 4), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMat4("model", model);
		ball.DrawMesh(0);
		shader.setBool("drawSmoothObject", false);
		if (cameraOption == followingMovingObject)
			cameraPos.x = movingBallX - 8.0f;
		if (cameraOption == watchingMovingObject)
			cameraFront = glm::vec3(movingBallX - cameraPos.x, 0.0f, -2.2f - cameraPos.z);
		
		// light sources
		liightSourceShader.use();
		liightSourceShader.setMat4("view", view);
		// moving light source
		model = glm::translate(glm::mat4(1.0f), glm::vec3(movingBallX, -1.0 + vibrations, -2.2f + 0.45f));
		model = glm::scale(model, glm::vec3(0.1f));
		liightSourceShader.setMat4("model", model);
		ball.DrawMesh(0);
		shader.use();
		shader.setVec3("light.position", 10.0f * sin((float)glfwGetTime() / 4) - 5.0f, -1.0 + vibrations, -2.2f + 0.45f);

		// static light sources
		float lightHeight = 0.2f;

		liightSourceShader.use();
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, -1.5f + lightHeight, 4.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		liightSourceShader.setMat4("model", model);
		ball.DrawMesh(0);
		shader.use();
		shader.setVec3("pointLight.position", -10.0f, -1.5f + lightHeight, 4.0f);

		liightSourceShader.use();
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, -1.5f + lightHeight, -4.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		liightSourceShader.setMat4("model", model);
		ball.DrawMesh(0);
		shader.use();
		shader.setVec3("secondPointLight.position", -10.0f, -1.5f + lightHeight, -4.0f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

Shader createMainShader(const char* vertexShader, const char* fragmentShader)
{
	Shader shader(vertexShader, fragmentShader);
	
	shader.use();

	shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	shader.setInt("objectShine", 32);

	// spot light struct values
	shader.setFloat("light.constant", 1.0f);
	shader.setFloat("light.linear", 0.0014);
	shader.setFloat("light.quadratic", 0.00007);
	shader.setVec3("light.position", 10.0f * sin((float)glfwGetTime() / 4) - 5.0f, -1.0f, -2.2f);
	shader.setVec3("light.direction", 0.0f, 0.0f, 1.0f);

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

	shader.setVec3("pointLight.color", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLight.ambient", 0.1f);
	shader.setFloat("pointLight.specular", 0.5f);
	shader.setFloat("pointLight.diffuse", 1.0f);

	// second point light struct values
	shader.setVec3("secondPointLight.position", cameraPos.x, cameraPos.y, cameraPos.z + 100);
	// attenuation
	shader.setFloat("secondPointLight.constant", 1.0f);
	shader.setFloat("secondPointLight.linear", 0.0014);
	shader.setFloat("secondPointLight.quadratic", 0.000007);

	shader.setVec3("secondPointLight.color", 1.0f, 1.0f, 1.0f);
	shader.setFloat("secondPointLight.ambient", 0.1f);
	shader.setFloat("secondPointLight.specular", 0.5f);
	shader.setFloat("secondPointLight.diffuse", 1.0f);

	// night factor
	shader.setFloat("nightFactor", 0.0f);

	shader.setBool("blinn", false);

	return shader;
}