#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <learnopengl\filesystem.h>
#include "shader_s.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>

#define window_size_x 800
#define window_size_y 600
using namespace std;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 1.4f);

// camera
glm::vec3 camera(0.0f, 0.0f, 3.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	glfwInit();
	// ��ʼ��GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// �������ڶ���
	GLFWwindow* window = glfwCreateWindow(window_size_x, window_size_y, "OpenGLTest", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// ����OpenGL��Ⱦ���ڵĳߴ��С����glViewport�������ô���ά��
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// �ڵ���OpenGL����ǰ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	// ����Ϊһ����ɫ���������
	Shader Phong_lightingShader("vertexShader.vs", "fragmentShader.fs");
	Shader Gouraud_lightingShader("vertexShader2.vs", "fragmentShader2.fs");
	Shader lampShader("lamp.vs", "lamp.fs");
	// ��������ImGui
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	bool is_phong_shading = true, is_gouraud_shading = false;

	float vertices[] = {
		// ��������			  // ������
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

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	// ��VAO
	glBindVertexArray(cubeVAO);
	// ���ƶ������鵽���幩OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// ����λ������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ���÷���������ָ��
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	float width = 32.0f, height = 24.0f;
	float radian = 45.0f;
	int shininess = 32; // �����
	float ambientStrength = 0.1f; // ambient����
	float specularStrength = 1.0f; //specular����

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		// ����
		processInput(window);
		glfwPollEvents();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glDisable(GL_DEPTH_TEST);

		// ����GUI�˵���
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::SetWindowFontScale(1.4);
		ImGui::Text("Choose Shading");
		ImGui::Checkbox("Show Phong Shading", &is_phong_shading);
		ImGui::Checkbox("Show Gouraud Shading", &is_gouraud_shading);

		//bind textures on corresponding texture units
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-2.0f, 2.0f, -1.5f));
		view = glm::translate(view, glm::vec3(-1.0f, -1.0f, -5.0f));
		projection = glm::perspective(glm::radians(radian), (float)window_size_x / (float)window_size_y, 0.1f, 100.0f);

		// ѡ��Phong Shading
		if (is_phong_shading)
		{
			ImGui::SliderFloat("ambient", &ambientStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("specular", &specularStrength, 0.0f, 1.0f);
			ImGui::SliderInt("shininess", &shininess, 0, 32);
			Phong_lightingShader.use();
			Phong_lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			Phong_lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			Phong_lightingShader.setVec3("lightPos", lightPos);
			Phong_lightingShader.setVec3("viewPos", camera);
			Phong_lightingShader.setInt("shininess", shininess);
			Phong_lightingShader.setFloat("ambientStrength", ambientStrength);
			Phong_lightingShader.setFloat("specularStrength", specularStrength);
			unsigned int modelLoc = glGetUniformLocation(Phong_lightingShader.ID, "model");
			unsigned int viewLoc = glGetUniformLocation(Phong_lightingShader.ID, "view");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(Phong_lightingShader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
			//Phong_lightingShader.use();
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ѡ��Gouraud Shading
		else if (is_gouraud_shading)
		{
			ImGui::SliderFloat("ambient", &ambientStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("specular", &specularStrength, 0.0f, 1.0f);
			ImGui::SliderInt("shininess", &shininess, 0, 32);
			Gouraud_lightingShader.use();
			Gouraud_lightingShader.setVec3("lightPos", lightPos);
			Gouraud_lightingShader.setVec3("viewPos", camera);
			Gouraud_lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			Gouraud_lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			Gouraud_lightingShader.setMat4("model", model);
			Gouraud_lightingShader.setMat4("view", view);
			Gouraud_lightingShader.setMat4("projection", projection);
			Gouraud_lightingShader.setInt("shininess", shininess);
			Gouraud_lightingShader.setFloat("ambientStrength", ambientStrength);
			Gouraud_lightingShader.setFloat("specularStrength", specularStrength);
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		ImGui::Render();
		
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}