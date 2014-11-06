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

ActionWindow *actWindow = nullptr;
RegionGrowing *regionGrowing = nullptr;

extern bool mousePressed[2];
//projection and modelview matrices
glm::mat4  P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);
glm::mat4 MVP_inv = glm::mat4(1);


Face *pickingFace = nullptr;
ProgStatus progStatus = PS_NORMAL;

float translate_x = 0, translate_y = 0;
//Here I'm going to try using the ImDrawVert..
GLuint vaoID, vboColorID, vboIndexID;
GLuint vboVerticesID;
GLint triangle_num;




static void prepareMeshData(Mesh *mesh) {
}


void OnRender() {
}


static void OnShutdown() {
	ShaderManager::terminate();
    //Destroy vao and vbo
    glDeleteBuffers(1, &vboVerticesID);
    glDeleteVertexArrays(1, &vaoID);
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

    /*regionGrowing = new RegionGrowing();
    regionGrowing->initPatches(mesh, 50);*/

	GUI::init();
	GUI::setDiameter(meshData->getDiameter());
	GUI::setCenter(meshData->getCenter());
	GLFWwindow *window = GUI::getWindow();

    ShaderManager::init();

	MeshRenderer *meshRenderer = new MeshRenderer();
	meshRenderer->loadData(meshData);

    InitImGui();

    actWindow = new ActionWindow(window);

    const float window_width = ImGui::GetIO().DisplaySize.x;

    const float window_height = ImGui::GetIO().DisplaySize.y;

	GLSLShader colorShader = ShaderManager::getShader("Color");
    while (!glfwWindowShouldClose(window))
    {

        ImGuiIO& io = ImGui::GetIO();
        mousePressed[0] = mousePressed[1] = false;
        io.MouseWheel = 0;
        glfwPollEvents();
        UpdateImGui();


        static bool show_test_window = false;
        static bool show_another_window = false;

  

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (0 || show_another_window)
         {
            ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if ( 0 && show_test_window)
        {
            ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));        // Normally user code doesn't need/want to call this, because positions are saved in .ini file. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowTestWindow(&show_test_window);
        }

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
        meshRenderer->render();
        if (progStatus == PS_PICKING_SEED) {
            if (pickingFace) {
                ImGui::SetTooltip("Face %d", pickingFace->index());
            } else {
                ImGui::SetTooltip("     Picking    ");
            }

        }

        ImGui::Render();


        glfwSwapBuffers(window);
    }

  

    OnShutdown();
    UnLoadDrawingData();
    ImGui::Shutdown();
   
    delete actWindow;
    delete meshData;
	delete meshRenderer;
    exit(EXIT_SUCCESS);
}