#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);
float bernstein(int i, int n, float t);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
double mousex = 0;
double mousey = 0;
float points[100];
float processp[100];
int n = -1;
float t = 0;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos, 0.0f, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);\n"
"}\0";


int main() {
	//初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//对窗口注册回调函数，改变窗口大小时，调整视口
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	/***************构造着色器程序*****************/
	//顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//链接着色器
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/*************创建VAO,VBO*********/
	

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	/****************变量**************************/
	float vertices[202];
	float tempx, tempy;

	/******************循环渲染*********************/
	while (!glfwWindowShouldClose(window)) {
		t += 0.0001;
		if (t > 1) t = 0.0;

		//处理输入
		processInput(window);

		//渲染
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//给曲线顶点数据赋值
		for (float t = 0; t <= 100; t += 1.0f) {
			tempx = tempy = 0;
			for (int i = 0; i <= n; i++) {
				float b = bernstein(i, n, t / 100);
				tempx += points[2 * i] * b;
				tempy += points[2 * i + 1] * b;
			}
			vertices[int(2 * t)] = tempx;
			vertices[int(2 * t + 1)] = tempy;
		}

		//绘制点
		glPointSize(10);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 2 * (n + 1) * sizeof(float), points, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glUseProgram(shaderProgram);
		glDrawArrays(GL_POINTS, 0, n + 1);
		glDrawArrays(GL_LINE_STRIP, 0, n + 1);


		//绘制曲线
		if (n >= 1) {
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glUseProgram(shaderProgram);
			glDrawArrays(GL_LINE_STRIP, 0, 101);
		}
		
		//绘制过程
		for (int i = 0; i <= 2 * n + 1; i++) {
			processp[i] = points[i];
		}
		for (int k = n; k > 0; k--) {
			for (int i = 0; i < k; i++) {
				processp[2 * i] = (1 - t) * processp[2 * i] + t * processp[2 * (i + 1)];
				processp[2 * i + 1] = (1 - t) * processp[2 * i + 1] + t * processp[2 * (i + 1) + 1];
			}
			glPointSize(5);
			glBufferData(GL_ARRAY_BUFFER, 2 * k * sizeof(float), processp, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glUseProgram(shaderProgram);
			glDrawArrays(GL_POINTS, 0, k);
			glDrawArrays(GL_LINE_STRIP, 0, k);
		}
		


		//交换缓冲，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//释放资源，退出程序
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();


	return 0;
}


/*********处理输入*************/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void processInput(GLFWwindow *window)
{	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mousex = (xpos - SCR_WIDTH / 2) / SCR_WIDTH * 2;
	mousey = (SCR_HEIGHT / 2 - ypos) / SCR_HEIGHT * 2;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			n++;
			points[2 * n] = mousex;
			points[2 * n + 1] = mousey;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && n >= 0) {
			n--;
		}
	}
}

float bernstein(int i, int n, float t) {
	int coef = 1;
	for (int k = 2; k <= n; k++) {
		coef *= k;
	}
	for (int k = 2; k <= i; k++) {
		coef /= k;
	}
	for (int k = 2; k <= n - i; k++) {
		coef /= k;
	}
	return coef * pow(t, i) * pow(1 - t, n - i);
}
