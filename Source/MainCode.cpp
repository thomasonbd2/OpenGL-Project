#include <iostream>         // error handling and output
#include <cstdlib>          // EXIT_FAILURE

#include <GL/glew.h>        // GLEW library
#include "GLFW/glfw3.h"     // GLFW library

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

// Namespace for declaring global variables
namespace
{
    const char* const WINDOW_TITLE = "Final Project";
    GLFWwindow* g_Window = nullptr;
    SceneManager* g_SceneManager = nullptr;
    ShaderManager* g_ShaderManager = nullptr;
    ViewManager* g_ViewManager = nullptr;
}

// Function declarations
bool InitializeGLFW();
bool InitializeGLEW();

int main(int argc, char* argv[])
{
    if (!InitializeGLFW()) return EXIT_FAILURE;

    g_ShaderManager = new ShaderManager();
    g_ViewManager = new ViewManager(g_ShaderManager);
    g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);
    if (!g_Window) return EXIT_FAILURE;

    if (!InitializeGLEW()) return EXIT_FAILURE;

    g_ShaderManager->LoadShaders(
        "../../Utilities/shaders/vertexShader.glsl",
        "../../Utilities/shaders/fragmentShader.glsl");
    g_ShaderManager->use();

    g_SceneManager = new SceneManager(g_ShaderManager);
    g_SceneManager->PrepareScene();

    // === ImGui Setup ===
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // === GUI state ===
    bool wireframe = false;
    static int viewMode = 0; // 0 = Perspective, 1 = Orthographic, 2 = Top, 3 = Side, 4 = Front

    // Default view
    g_ViewManager->SetPerspectiveView();

    while (!glfwWindowShouldClose(g_Window))
    {
        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI Controls
        ImGui::Begin("Scene Controls");

        // Wireframe toggle
        ImGui::Checkbox("Wireframe Mode", &wireframe);

        // Camera speed slider
        float camSpeed = g_ViewManager->GetCameraSpeed();
        if (ImGui::SliderFloat("Camera Speed", &camSpeed, 0.1f, 5.0f))
        {
            g_ViewManager->SetCameraSpeed(camSpeed);
        }

        glm::vec3 camPos = g_ViewManager->GetCameraPosition();
        ImGui::Separator();
        ImGui::Text("Camera Position: X: %.2f Y: %.2f Z: %.2f", camPos.x, camPos.y, camPos.z);



        ImGui::Separator();
        ImGui::Text("Camera Controls:");
        ImGui::Text("W/S/A/D - Move");
        ImGui::Text("Q/E - Up/Down");
        ImGui::Text("Mouse - Look");
        ImGui::Text("Scroll - Adjust Speed");
        ImGui::Text("ESC - Exit");

        ImGui::Separator();
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();

        // Apply wireframe toggle
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        // Clear the frame
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Prepare scene and render
        g_ViewManager->PrepareSceneView();
        g_SceneManager->RenderScene();

        // Render ImGui on top
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_Window);
        glfwPollEvents();
    }

    // === ImGui Cleanup ===
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Clean up managers
    delete g_SceneManager;
    delete g_ViewManager;
    delete g_ShaderManager;

    exit(EXIT_SUCCESS);
}

// GLFW setup
bool InitializeGLFW()
{
    glfwInit();
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    return true;
}

// GLEW setup
bool InitializeGLEW()
{
    GLenum GLEWInitResult = glewInit();
    if (GLEW_OK != GLEWInitResult)
    {
        std::cerr << glewGetErrorString(GLEWInitResult) << std::endl;
        return false;
    }
    std::cout << "INFO: OpenGL Successfully Initialized\n";
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << "\n\n";
    return true;
}
