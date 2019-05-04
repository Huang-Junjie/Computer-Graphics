#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


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
	glfwSetScrollCallback(window, scroll_callback);


	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

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

	bool isOrthographic = false;
	float vOrthographic[4] = {-100.0f, 100.0f, -100.0f, 100.0f};
	float vOrthographic2[2] = {0.1f, 100.0f};

	bool isPerspective = false;
	float vPerspective[2] = {0.1f, 100.0f};

	bool isAutoRotation = false;
	bool isFPS = false;

	/******************循环渲染*********************/
	while (!glfwWindowShouldClose(window)) {
		//计算时间差，平衡移动速度
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//处理输入
		processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settting");
		ImGui::Checkbox("orthographic projection)", &isOrthographic);
		ImGui::SliderFloat4("(left, right, boottom, top)", vOrthographic, -100.0f, 100.0f);
		ImGui::SliderFloat2("(near, far)", vOrthographic2, 0.0f, 100.0f);

		ImGui::Checkbox("perspective projection)", &isPerspective);
		ImGui::SliderFloat2("(near, far)", vPerspective, 0.0f, 100.0f);
		ImGui::SliderFloat("(fov)", &fov, 1.0f, 90.0f);

		ImGui::Checkbox("Auto Rotation", &isAutoRotation);
		ImGui::Checkbox("FPS", &isFPS);


		if (isFPS) {
			isAutoRotation = false;
			isPerspective = false;
			isOrthographic = false;
			// tell GLFW to capture our mouse
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(window, mouse_callback);
		}
		ImGui::End();

		//坐标变换矩阵
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		if (isOrthographic) {
			projection = glm::ortho(vOrthographic[0], vOrthographic[1], vOrthographic[2], vOrthographic[3], 
									vOrthographic2[0], vOrthographic2[1]);
		}
		if (isPerspective) {
			projection = projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 
													vPerspective[0], vPerspective[1]);
		}

		if (isAutoRotation) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			float camX = sin(glfwGetTime()) * 20;
			float camZ = cos(glfwGetTime()) * 20;
			view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		}

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

		float colorX[6] = { 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
		float colorY[6] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f };
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBack(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(deltaTime);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
	camera.rotate(yoffset, xoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}