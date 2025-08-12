#include "ViewManager.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <json.hpp>
#include <DebugLogger.h>

using json = nlohmann::json;

// Global variables (internal)
namespace
{
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	Camera* g_pCamera = nullptr;
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	float gDeltaTime = 0.0f;
	float gLastFrame = 0.0f;
	bool bOrthographicProjection = false;
	float gCameraSpeed = 2.5f;
}

ViewManager::ViewManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_pWindow = nullptr;
	g_pCamera = new Camera();

	// Default camera position
	g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
	g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 80;

}

ViewManager::~ViewManager()
{
	m_pShaderManager = nullptr;
	m_pWindow = nullptr;
	if (g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = nullptr;
	}
}

GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);
	glfwSetScrollCallback(window, &ViewManager::Mouse_Scroll_Callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;
	return window;
}

void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	if (gFirstMouse)
	{
		gLastX = xMousePos;
		gLastY = yMousePos;
		gFirstMouse = false;
	}

	float xOffset = xMousePos - gLastX;
	float yOffset = gLastY - yMousePos;

	gLastX = xMousePos;
	gLastY = yMousePos;

	g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}

void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (yOffset > 0) gCameraSpeed += 0.1f;
	else if (yOffset < 0) gCameraSpeed -= 0.1f;

	if (gCameraSpeed < 0.1f) gCameraSpeed = 0.1f;
	if (gCameraSpeed > 5.0f) gCameraSpeed = 5.0f;
}

void ViewManager::ProcessKeyboardEvents()
{
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_pWindow, true);

	if (!g_pCamera) return;

	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS) g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime * gCameraSpeed);
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS) g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime * gCameraSpeed);
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS) g_pCamera->ProcessKeyboard(LEFT, gDeltaTime * gCameraSpeed);
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS) g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime * gCameraSpeed);
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS) g_pCamera->ProcessKeyboard(UP, gDeltaTime * gCameraSpeed);
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS) g_pCamera->ProcessKeyboard(DOWN, gDeltaTime * gCameraSpeed);

	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS) SetOrthographicView();
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS) SetPerspectiveView();
}

void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	ProcessKeyboardEvents();

	view = g_pCamera->GetViewMatrix();

	if (!bOrthographicProjection)
	{
		projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else
	{
		double scale = (WINDOW_WIDTH > WINDOW_HEIGHT) ?
			(double)WINDOW_HEIGHT / (double)WINDOW_WIDTH :
			(double)WINDOW_WIDTH / (double)WINDOW_HEIGHT;

		if (WINDOW_WIDTH > WINDOW_HEIGHT)
			projection = glm::ortho(-5.0f, 5.0f, -5.0f * (float)scale, 5.0f * (float)scale, 0.1f, 100.0f);
		else if (WINDOW_WIDTH < WINDOW_HEIGHT)
			projection = glm::ortho(-5.0f * (float)scale, 5.0f * (float)scale, -5.0f, 5.0f, 0.1f, 100.0f);
		else
			projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	}

	if (m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ViewName, view);
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
	}

	int fps = (gDeltaTime > 0) ? static_cast<int>(1.0f / gDeltaTime) : 0;
	std::string lastKeyPress = "None";
	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS) lastKeyPress = "W";
	else if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS) lastKeyPress = "S";
	else if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS) lastKeyPress = "A";
	else if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS) lastKeyPress = "D";

	g_DebugLogger.LogFrame(fps, lastKeyPress);
	g_DebugLogger.Update(gDeltaTime);
}

void ViewManager::SetPerspectiveView()
{
	bOrthographicProjection = false;
	g_pCamera->Position = glm::vec3(0.0f, 5.5f, 8.0f);
	g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 80.0f;
}

void ViewManager::SetOrthographicView()
{
	bOrthographicProjection = true;
	g_pCamera->Position = glm::vec3(0.0f, 4.0f, 10.0f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Front = glm::vec3(0.0f, 0.0f, -1.0f);
}

Camera* ViewManager::GetCamera() const { return g_pCamera; }
float ViewManager::GetCameraSpeed() const { return gCameraSpeed; }
bool ViewManager::IsOrthographic() const { return bOrthographicProjection; }
void ViewManager::SetCameraSpeed(float speed) { gCameraSpeed = glm::clamp(speed, 0.1f, 5.0f); }
glm::vec3 ViewManager::GetCameraPosition() const {
	return g_pCamera ? g_pCamera->Position : glm::vec3(0.0f);
}


