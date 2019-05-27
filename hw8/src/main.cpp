#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"
#include <iostream>

#define window_size_x 800
#define window_size_y 600
#define bezier_point_num 1000
using namespace std;

float xpos = 0, ypos = 0;
int point_num = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

struct points_array
{
	float *x;
	float *y;
	int index = 0; // ��ǰ���Ƶ�ĸ���
};

//�������Ƶ�
points_array points;

//���λ�ûص�����
void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
	//xpos = float((x - window_size_x / 2) / window_size_x) * 2;
	//ypos = float(0 - (y - window_size_y / 2) / window_size_y) * 2;
	xpos = float((x - (window_size_x/2)) / (window_size_x/2)) * 1;
	ypos = float(((window_size_y/2) - y) / (window_size_y/2)) * 1;
	return;
}

//������ص�����
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		//���������
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			point_num++;
			points.index = point_num;
			//cout << "xPos: " << xpos << "  yPos: " << ypos << endl;
			points.x[points.index - 1] = xpos;
			points.y[points.index - 1] = ypos;
		}
		//�������Ҽ�
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (point_num > 0)
			{
				point_num--;
				points.index = point_num;
			}
		}
	}
	return;
}


//��n�Ľ׳�
int JieCheng(int n)
{
	if (n == 1 || n == 0)
	{
		return 1;
	}
	else
	{
		return n * JieCheng(n - 1);
	}
}

//���������
float C(int n, int i)
{
	return ((float)JieCheng(n)) / ((float)(JieCheng(i)*JieCheng(n - i)));
}

//�󸡵�����n�η�
float N(float u, int n)
{
	float sum = 1.0;
	if (n == 0)
	{
		return 1;
	}
	for (int i = 0; i < n; i++)
	{
		sum *= u;
	}
	return sum;
}

points_array bezier()
{
	points_array p;
	p.x = new float[bezier_point_num];
	p.y = new float[bezier_point_num];
	int num = 0;
	for (float i = 0; i < 1; i = i + 0.001)
	{
		float x = 0, y = 0;
		for (int j = 0; j < point_num; j++)
		{
			x += C(point_num - 1, j) * N(i, j) * N(1 - i, point_num - 1 - j) * points.x[j];
			y += C(point_num - 1, j) * N(i, j) * N(1 - i, point_num - 1 - j) * points.y[j];
		}
		p.x[num] = x;
		p.y[num] = y;
		num++;
	}
	return p;
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
	// ������ɫ��Դ����
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"   ourColor = aColor;\n"
		"}\0";
	// ������ɫ��
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// ����Ƿ����ɹ�
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// Ƭ����ɫ������
	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(ourColor, 1.0f);\n"
		"}\n\0";
	// ������ɫ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// ����Ϊһ����ɫ���������
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// ������Ӵ���
	// ɾ����ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	points.x = new float[100];
	points.y = new float[100];

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		// ����
		processInput(window);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);

	
		// ����ֱ��
		if (point_num >= 2)
		{
			float *line_vertices = new float[6 * 2 * (point_num - 1)];
			//cout << point_num << endl;
			for (int i = 0, k = 0; i < point_num - 1; )
			{
				line_vertices[k * 6 + 0] = points.x[i];
				line_vertices[k * 6 + 1] = points.y[i];
				line_vertices[k * 6 + 2] = 0.0f;
				line_vertices[k * 6 + 3] = 0.0f;
				line_vertices[k * 6 + 4] = 0.0f;
				line_vertices[k * 6 + 5] = 1.0f;
				i++;
				k++;
				line_vertices[k * 6 + 0] = points.x[i];
				line_vertices[k * 6 + 1] = points.y[i];
				line_vertices[k * 6 + 2] = 0.0f;
				line_vertices[k * 6 + 3] = 0.0f;
				line_vertices[k * 6 + 4] = 0.0f;
				line_vertices[k * 6 + 5] = 1.0f;
				k++;
				//cout << i << " " << k << endl;
			}

			// ��VAO
			glBindVertexArray(VAO);
			// ���ƶ������鵽���幩OpenGLʹ��
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, 2 * (point_num - 1) * 6 * sizeof(float), line_vertices, GL_STATIC_DRAW);
			// ���ö�������ָ��
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// ������ɫ����ָ��
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// �����е㶼������
			//glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, 2 * (point_num - 1));
		}
		
		// ����Ҫ3�����Ƶ�
		if (point_num >= 3)
		{
			float *vertices = new float[6 * bezier_point_num];
			points_array bezier_points = bezier();
			for (int i = 0; i < bezier_point_num; i++)
			{
				vertices[i * 6 + 0] = bezier_points.x[i];
				vertices[i * 6 + 1] = bezier_points.y[i];
				vertices[i * 6 + 2] = 0.0f;
				vertices[i * 6 + 3] = 0.0f;
				vertices[i * 6 + 4] = 0.0f;
				vertices[i * 6 + 5] = 1.0f;
			}

			// ��VAO
			glBindVertexArray(VAO);
			// ���ƶ������鵽���幩OpenGLʹ��
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, bezier_point_num * 6 * sizeof(float), vertices, GL_STATIC_DRAW);
			// ���ö�������ָ��
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// ������ɫ����ָ��
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// �����е㶼������
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, bezier_point_num);

			
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}