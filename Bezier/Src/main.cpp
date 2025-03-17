#include <glad/glad.h>  
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

GLuint CompileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
std::vector<float> generateBezierPoint(const std::vector<float>& controlPoints, float t);
std::vector<float> generateBezierPoints(const std::vector<float>& controlPoints);

std::vector<float> coordinates;
bool acceptInput = true;

int main() {
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置 GLFW 窗口属性
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建 GLFW 窗口
    GLFWwindow* window = glfwCreateWindow(1600, 1200, "GLFW & GLAD Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 设置当前上下文
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 打印 OpenGL 版本
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    GLuint shader = CompileShader(vertexShaderSource, fragmentShaderSource);
	glUseProgram(shader);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexArrayAttrib(VAO, 0);

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        // 清屏
        glClear(GL_COLOR_BUFFER_BIT);

		if (!acceptInput)
		{
			std::vector<float> bezierPoints = generateBezierPoints(coordinates);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, bezierPoints.size() * sizeof(float), bezierPoints.data(), GL_STATIC_DRAW);
			// 使用着色器程序
			glUseProgram(shader);

			// 绑定 VAO
			glBindVertexArray(VAO);

			// 绘制线段
			glDrawArrays(GL_LINE_STRIP, 0, bezierPoints.size() / 3);

		}


        // 交换缓冲区
        glfwSwapBuffers(window);

        // 处理事件
        glfwPollEvents();
    }

    // 清理并退出
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLuint CompileShader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	// 编译顶点着色器
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	// 检查顶点着色器是否编译成功
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// 编译片段着色器
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	// 检查片段着色器是否编译成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// 链接着色器程序
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 检查着色器程序是否链接成功
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// 删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (acceptInput)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            // 获取鼠标位置
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            // 获取窗口大小
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            // 将屏幕坐标转换为标准化设备坐标（NDC）
            float ndcX = (2.0f * xpos) / width - 1.0f;
            float ndcY = 1.0f - (2.0f * ypos) / height;
            float ndcZ = 0.0f; // 假设 Z 坐标为 0

            // 将坐标添加到数组
            coordinates.push_back(ndcX);
            coordinates.push_back(ndcY);
            coordinates.push_back(ndcZ);

            // 打印当前坐标
            std::cout << "Added point: (" << ndcX << ", " << ndcY << ", " << ndcZ << ")" << std::endl;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            // 右键点击时停止接收输入
            acceptInput = false;
        }
    }
}

std::vector<float> generateBezierPoint(const std::vector<float>& controlPoints, float t)
{
	if (controlPoints.size() > 3)
	{
		std::vector<float> newControlPoints;
		for (int i = 0; i < controlPoints.size() - 3; i += 3)
		{
			float x = (1 - t) * controlPoints[i] + t * controlPoints[i + 3];
			float y = (1 - t) * controlPoints[i + 1] + t * controlPoints[i + 4];
			float z = (1 - t) * controlPoints[i + 2] + t * controlPoints[i + 5];
			newControlPoints.push_back(x);
			newControlPoints.push_back(y);
			newControlPoints.push_back(z);
		}
		return generateBezierPoint(newControlPoints, t);
	}
	else
	{
		return controlPoints;
	}
}

std::vector<float> generateBezierPoints(const std::vector<float>& controlPoints)
{
	const int numPoints = 100;
	float stride = 1.0f / numPoints;
	std::vector<float> bezierPoints;
	for (float t = 0.0f; t <= 1.0f; t += stride)
	{
		std::vector<float> point = generateBezierPoint(controlPoints, t);
		bezierPoints.insert(bezierPoints.end(), point.begin(), point.end());
	}
	return bezierPoints;
}
