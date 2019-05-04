#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"	FragColor = ourColor;\n"
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
	float vertices[] = {
	-2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f, -2.0f,
	 2.0f,  2.0f, -2.0f,
	 2.0f,  2.0f, -2.0f,
	-2.0f,  2.0f, -2.0f,
	-2.0f, -2.0f, -2.0f,

	-2.0f, -2.0f,  2.0f,
	 2.0f, -2.0f,  2.0f,
	 2.0f,  2.0f,  2.0f,
	 2.0f,  2.0f,  2.0f,
	-2.0f,  2.0f,  2.0f,
	-2.0f, -2.0f,  2.0f,

	-2.0f,  2.0f,  2.0f,
	-2.0f,  2.0f, -2.0f,
	-2.0f, -2.0f, -2.0f,
	-2.0f, -2.0f, -2.0f,
	-2.0f, -2.0f,  2.0f,
	-2.0f,  2.0f,  2.0f,

	 2.0f,  2.0f,  2.0f,
	 2.0f,  2.0f, -2.0f,
	 2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f,  2.0f,
	 2.0f,  2.0f,  2.0f,

	-2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f, -2.0f,
	 2.0f, -2.0f,  2.0f,
	 2.0f, -2.0f,  2.0f,
	-2.0f, -2.0f,  2.0f,
	-2.0f, -2.0f, -2.0f,

	-2.0f,  2.0f, -2.0f,
	 2.0f,  2.0f, -2.0f,
	 2.0f,  2.0f,  2.0f,
	 2.0f,  2.0f,  2.0f,
	-2.0f,  2.0f,  2.0f,
	-2.0f,  2.0f, -2.0f
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


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

	bool isEnableDepthTest = false;
	bool isAutoTranslation = false;
	bool isAutoRotation = false;
	bool isAutoScaling = false;
	float translation[3] = { 0 };
	float angle = 0;
	float scale = 1;

	/******************循环渲染*********************/
	while (!glfwWindowShouldClose(window)) {
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settting");
		ImGui::Checkbox("Enable Deep Test", &isEnableDepthTest);
		ImGui::Checkbox("Auto Rotation", &isAutoRotation);
		ImGui::Checkbox("Auto Translation", &isAutoTranslation);
		ImGui::Checkbox("Auto Scaling", &isAutoScaling);
		ImGui::SliderFloat3("Translation", translation, -10.0f, 10.0f);
		ImGui::SliderFloat("Angle", &angle, 0.0f, 360.0f);
		ImGui::SliderFloat("Scale", &scale, 0.0f, 2.0f);
		ImGui::End();
		


		if (isEnableDepthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		if (isAutoTranslation) {
			translation[0] = 10 * (float)glfwGetTime();
			translation[0] -= (int)translation[0] / 40 * 40;
			if (translation[0] < 20) translation[0] -= 10;
			else translation[0] = 30 - translation[0];
			translation[1] = 0;
			translation[2] = 0;
		}

		if (isAutoRotation) {
			angle = (float)glfwGetTime();
			angle -= (int)angle / 360 * 360;
		}

		if (isAutoScaling) {
			scale = (float)glfwGetTime();
			scale -= (int)scale / 4 * 4;
			if (scale > 2) scale = 4 - scale;
		}

		//坐标变换矩阵
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(translation[0], translation[1], translation[2]));
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		//渲染
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glBindVertexArray(VAO);
		glUseProgram(shaderProgram);
		int loc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));
		loc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
		loc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

		float colorX[6] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f };
		float colorY[6] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
		float colorZ[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };
		for (int i = 0; i < 6; i++) {
			loc = glGetUniformLocation(shaderProgram, "ourColor");
			glUniform4f(loc, colorX[i], colorY[i], colorZ[i], 1.0f);
			glDrawArrays(GL_TRIANGLES, 6 * i, 6);
		}

		//交换缓冲，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//释放资源，退出程序
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}