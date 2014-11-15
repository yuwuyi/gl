#include <iostream>
#include "GLSLShader.h"
#include "GLFW/glfw3.h"
#include "imgui_stuff.h"
#include "imgui/imgui.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MeshData.h"
#include "Renderer.h"
#include "growing/growing.h"

#include "GUI.h"

#include "ActionWindow.h"
#include "ShaderManager.h"
#include "RendererManager.h"
#include "ScenarioManager.h"
#include "MeshDataManager.h"

ActionWindow *actWindow = nullptr;
RegionGrowing *regionGrowing = nullptr;


extern bool mousePressed[2];



static void OnShutdown() {
	ShaderManager::terminate();
    
    GUI::terminate();
    MeshDataManager::terminate();
    RendererManager::terminate();
    cout<<"Shutdown successfull"<<endl;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " input.m\n";
        exit(-1);
    }

    Mesh *mesh = new Mesh;
    mesh->readMFile(argv[1]);
	MeshData *meshData = new MeshData(mesh);
	meshData->buildKDTree();
	meshData->calculateCenter();

    
	GUI::init();
	GUI::setDiameter(meshData->getDiameter());
	GUI::setCenter(meshData->getCenter());
	GLFWwindow *window = GUI::getWindow();

    ShaderManager::init();

	MeshRenderer *meshRenderer = new MeshRenderer();
	meshRenderer->loadData(meshData);

    MeshDataManager::addMeshData(meshData);
    MeshDataManager::setCurrent(meshData);
    
    RendererManager::addRenderer(meshRenderer);
    
    
    ScenarioManager::init();
    InitImGui();

    actWindow = new ActionWindow(window);

	GLSLShader colorShader = ShaderManager::getShader("Color");
    
    while (!glfwWindowShouldClose(window))
    {

        ImGuiIO& io = ImGui::GetIO();
        mousePressed[0] = mousePressed[1] = false;
        io.MouseWheel = 0;
        glfwPollEvents();
        UpdateImGui();


		//here is the mode window..
		if (1) {
			const ImGuiWindowFlags layout_flags 
				= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoMove	| ImGuiWindowFlags_NoResize;
			
			ImGuiStyle& style = ImGui::GetStyle();
			float back_w = style.Colors[ImGuiCol_WindowBg].w;
			style.Colors[ImGuiCol_WindowBg].w = 0;

			ImGui::Begin("Mode", nullptr, ImVec2(200,20), 0.6, layout_flags);
			ImGui::SetWindowPos(ImVec2(0, 0)); 
			ImGui::TextColored(ImVec4(1, 1, 0, 1),"Mode:");
			ImGui::End();
			style.Colors[ImGuiCol_WindowBg].w = back_w;
		}

        {
            if (actWindow->isShown()) {
               actWindow->render();
            }
        }

        // Rendering
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        //glClearColor(0.8f, 0.6f, 0.6f, 1.0f);
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

  
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        RendererManager::render();
        
        ScenarioManager::getCurrentScenario();
        
        
        ImGui::Render();


        glfwSwapBuffers(window);
    }

  

    OnShutdown();
    UnLoadDrawingData();
    ImGui::Shutdown();
   
    delete actWindow;
    exit(EXIT_SUCCESS);
}