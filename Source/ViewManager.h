///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"
#include "GLFW/glfw3.h"

class ViewManager
{
public:
	ViewManager(ShaderManager* pShaderManager);
	~ViewManager();

	// Mouse callbacks
	static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);
	static void Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);

	// Create the display window
	GLFWwindow* CreateDisplayWindow(const char* windowTitle);

	// Prepare the scene (view + projection matrices)
	void PrepareSceneView();

	// View controls
	void SetPerspectiveView();
	void SetOrthographicView();

	// Accessors for ImGui or other systems
	Camera* GetCamera() const;
	float GetCameraSpeed() const;
	bool IsOrthographic() const;
	void SetCameraSpeed(float speed);

	glm::vec3 GetCameraPosition() const;

private:
	ShaderManager* m_pShaderManager;
	GLFWwindow* m_pWindow;

	void ProcessKeyboardEvents();

	enum class ViewType { Perspective, Orthographic, Top, Side, Front };
	ViewType m_CurrentView = ViewType::Perspective;

	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
	float m_AspectRatio = 16.0f / 9.0f;

	glm::vec3 m_CameraPos; // Camera position
};
