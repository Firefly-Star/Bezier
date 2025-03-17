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
    // ��ʼ�� GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // ���� GLFW ��������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ���� GLFW ����
    GLFWwindow* window = glfwCreateWindow(1600, 1200, "GLFW & GLAD Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // ���õ�ǰ������
    glfwMakeContextCurrent(window);

    // ��ʼ�� GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // ��ӡ OpenGL �汾
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

    // ��ѭ��
    while (!glfwWindowShouldClose(window)) {
        // ����
        glClear(GL_COLOR_BUFFER_BIT);

		if (!acceptInput)
		{
			std::vector<float> bezierPoints = generateBezierPoints(coordinates);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, bezierPoints.size() * sizeof(float), bezierPoints.data(), GL_STATIC_DRAW);
			// ʹ����ɫ������
			glUseProgram(shader);

			// �� VAO
			glBindVertexArray(VAO);

			// �����߶�
			glDrawArrays(GL_LINE_STRIP, 0, bezierPoints.size() / 3);

		}


        // ����������
        glfwSwapBuffers(window);

        // �����¼�
        glfwPollEvents();
    }

    // �����˳�
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLuint CompileShader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	// ���붥����ɫ��
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	// ��鶥����ɫ���Ƿ����ɹ�
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// ����Ƭ����ɫ��
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	// ���Ƭ����ɫ���Ƿ����ɹ�
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// ������ɫ������
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// �����ɫ�������Ƿ����ӳɹ�
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// ɾ����ɫ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (acceptInput)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            // ��ȡ���λ��
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            // ��ȡ���ڴ�С
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            // ����Ļ����ת��Ϊ��׼���豸���꣨NDC��
            float ndcX = (2.0f * xpos) / width - 1.0f;
            float ndcY = 1.0f - (2.0f * ypos) / height;
            float ndcZ = 0.0f; // ���� Z ����Ϊ 0

            // ��������ӵ�����
            coordinates.push_back(ndcX);
            coordinates.push_back(ndcY);
            coordinates.push_back(ndcZ);

            // ��ӡ��ǰ����
            std::cout << "Added point: (" << ndcX << ", " << ndcY << ", " << ndcZ << ")" << std::endl;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            // �Ҽ����ʱֹͣ��������
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
