#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <cmath>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"	FragColor = ourColor;\n"
"}\0";

//bresenham算法画直线
void bresenham(int x0, int y0, int x1, int y1, float points[], int& len) {
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = abs(y1 - y0);
	int dy2 = 2 * dy;
	int dy2minusdx2 = dy2 - 2 * dx;
	int p = dy2 - dx;

	points[len++] = steep ? y0 : x0;
	points[len++] = steep ? x0 : y0;
	int y = y0;
	int ystep = y0 < y1 ? 1 : -1;
	for (int x = x0 + 1; x <= x1; x++) {
		if (p <= 0) {
			p += dy2;
		}
		else {
			y += ystep;
			p += dy2minusdx2;
		}
		points[len++] = steep ? y : x;
		points[len++] = steep ? x : y;
	}
}

//bresenham 八分法画圆
void bresenham(int r, float points[], int& len) {
	int x = 0;
	int y = r; 
	int p = 3 - 2 * r;
	for (x = 0; x <= y; x++) {
		points[len++] = x;
		points[len++] = y;

		points[len++] = -x;
		points[len++] = y;

		points[len++] = x;
		points[len++] = -y;

		points[len++] = -x;
		points[len++] = -y;

		points[len++] = y;
		points[len++] = x;

		points[len++] = -y;
		points[len++] = x;
		
		points[len++] = y;
		points[len++] = -x;
		
		points[len++] = -y;
		points[len++] = -x;

		if (p <= 0) {
			p += 4 * x + 6;
		}
		else {
			p += 4 * (x - y) + 10;
			y--;
		}
	}
}
//将坐标归一化到[-1 , 1]
void normalize(float& a) {
	a = a / 5;
}


void showImgui(bool& showTriangle,
	bool& showLine,
	bool& showCircle,
	bool& showColor,
	ImVec4& Color,
	int lineV1[],
	int lineV2[],
	int v1[],
	int v2[],
	int v3[], 
	int& r) {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Menu
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Add Primitive"))
		{	
			if (ImGui::MenuItem("Triangle", NULL, &showTriangle)) {
				showLine = showCircle = false;
			}
			if (ImGui::MenuItem("Bresenham Line", NULL, &showLine)) {
				showTriangle = showCircle = false;
			}
			if (ImGui::MenuItem("Bresenham Circle", NULL, &showCircle)) {
				showLine = showTriangle = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Setting"))
		{
			ImGui::MenuItem("Color", NULL, &showColor);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	//Color
	if (showColor) {
		ImGui::Begin("Color Settting", &showColor, 0);
		ImGui::ColorEdit3("", (float*)&Color);
		ImGui::End();
	}
	if (showLine) {
		ImGui::Begin("Line Settting", &showLine, 0);
		ImGui::SliderInt2("v1 (x, y)", lineV1, -5, 5);
		ImGui::SliderInt2("v2 (x, y)", lineV2, -5, 5);
		ImGui::End();
	}
	if (showTriangle) {
		ImGui::Begin("Triangle Settting", &showTriangle, 0);
		ImGui::SliderInt2("v1 (x, y)", v1, -5, 5);
		ImGui::SliderInt2("v2 (x, y)", v2, -5, 5);
		ImGui::SliderInt2("v3 (x, y)", v3, -5, 5);
		ImGui::End();
	}
	if (showCircle) {
		ImGui::Begin("Circle Settting", &showTriangle, 0);
		ImGui::SliderInt("R", &r, 0, 5);
		ImGui::End();
	}

}



int main() {
	//初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//对窗口注册回调函数，改变窗口大小时，调整视口
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

	/*************创建VAO,VBO; 配置顶点属性*********/
	//顶点数据
	float points[200];
	int len = 0;
	//网格
	float Lines[100];
	int i = 0;
	for (float y = -1; y <= 1; y += float(1) / 5) {
		Lines[i++] = -1;
		Lines[i++] = y;
		Lines[i++] = 1;
		Lines[i++] = y;
	}
	for (float x = -1; x <= 1; x += float(1) / 5) {
		Lines[i++] = x;
		Lines[i++] = -1;
		Lines[i++] = x;
		Lines[i++] = 1;
	}
	//grid VOA VBO
	unsigned int VAOgrid, VBOgrid;
	glGenVertexArrays(1, &VAOgrid);
	glBindVertexArray(VAOgrid);
	glGenBuffers(1, &VBOgrid);
	glBindBuffer(GL_ARRAY_BUFFER, VBOgrid);
	glBufferData(GL_ARRAY_BUFFER, 88 * sizeof(float), Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//顶点 VOA VBO
	unsigned int VAOpoint, VBOpoint;
	glGenVertexArrays(1, &VAOpoint);
	glGenBuffers(1, &VBOpoint);

	/*****************配置imgui***************/
	 // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImVec4 Color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	int v1[2] = { 0 };
	int v2[2] = { 0 };
	int v3[2] = { 0 };
	int lineV1[2] = { 0 };
	int lineV2[2] = { 0 };
	int r = 0;
	bool showTriangle = false;
	bool showLine = false;
	bool showCircle = false;
	bool showColor = false;

	//设置点的大小
	glPointSize(40);
	/******************循环渲染*********************/
	while (!glfwWindowShouldClose(window)) {
		//渲染
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//ImgUi
		showImgui(showTriangle, showLine, 
					showCircle, showColor, Color,
					lineV1, lineV2, v1, v2, v3, r);
		
		//grid
		glBindVertexArray(VAOgrid);
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 44);

		//line
		if (showLine) {
			len = 0;
			bresenham(lineV1[0], lineV1[1], lineV2[0], lineV2[1], points, len);
			for (int i = 0; i < len; i++) normalize(points[i]);
			glBindVertexArray(VAOpoint);
			glBindBuffer(GL_ARRAY_BUFFER, VBOpoint);
			glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), points, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
			glUseProgram(shaderProgram);
			glUniform4f(vertexColorLocation, Color.x, Color.y, Color.z, Color.w);
			glDrawArrays(GL_POINTS, 0, len / 2);
		}

		//triangle
		if (showTriangle) {
			len = 0;
			bresenham(v1[0], v1[1], v2[0], v2[1], points, len);
			bresenham(v1[0], v1[1], v3[0], v3[1], points, len);
			bresenham(v2[0], v2[1], v3[0], v3[1], points, len);
			for (int i = 0; i < len; i++) normalize(points[i]);
			glBindVertexArray(VAOpoint);
			glBindBuffer(GL_ARRAY_BUFFER, VBOpoint);
			glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), points, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
			glUseProgram(shaderProgram);
			glUniform4f(vertexColorLocation, Color.x, Color.y, Color.z, Color.w);
			glDrawArrays(GL_POINTS, 0, len / 2);
		}
		
		//circle
		if (showCircle) {
			len = 0;
			bresenham(r, points, len);
			for (int i = 0; i < len; i++) normalize(points[i]);
			glBindVertexArray(VAOpoint);
			glBindBuffer(GL_ARRAY_BUFFER, VBOpoint);
			glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), points, GL_STREAM_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
			glUseProgram(shaderProgram);
			glUniform4f(vertexColorLocation, Color.x, Color.y, Color.z, Color.w);
			glDrawArrays(GL_POINTS, 0, len / 2);
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//交换缓冲，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//释放资源，退出程序
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &VAOgrid);
	glDeleteBuffers(1, &VBOgrid);
	glDeleteVertexArrays(1, &VAOpoint);
	glDeleteBuffers(1, &VBOpoint);
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}