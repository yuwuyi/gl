#include <iostream>
#include "GLSLShader.h"
#include "GLFW/glfw3.h"
#include "imgui_stuff.h"
#include "imgui/imgui.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MeshLib_Core/Mesh.h"
#include "MeshLib_Core/Iterators.h"

#include "kdtree/kdtree.h"
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

glm::vec2 center;
GLfloat  diameter;
struct kdtree* ptree = nullptr;
struct kdres *presults;

Face *pickingFace = nullptr;
ProgStatus progStatus = PS_NORMAL;

float translate_x = 0, translate_y = 0;
//Here I'm going to try using the ImDrawVert..
GLuint vaoID, vboColorID, vboIndexID;
GLuint vboVerticesID;
GLint triangle_num;

void constructKDTree(Mesh *mesh) {
    ptree = kd_create( 3 );

    /* add some random nodes to the tree (assert nodes are successfully inserted) */
    for(MeshVertexIterator mvit(mesh); !mvit.end(); ++mvit) {
        Vertex *v = *mvit;
        if ( 0 != kd_insert3( ptree, v->point()[0], v->point()[1], v->point()[2], v ) ) {
			std::cout << "kdtree fail!\n";
			exit(-1);
		}
    }
}


static void prepareMeshData(Mesh *mesh) {
	GLSLShader colorShader = ShaderManager::getShader("Color");
	//GLSLShader wireframeShader = ShaderManager::getShader("WireFrame");


    std::vector<glm::vec2> pos;
    std::vector<int> indices;
    std::vector<glm::vec4> colors;

    glm::vec2 minp(1e6, 1e6), maxp(-1e6, -1e6);

    for (MeshVertexIterator mvit(mesh); !mvit.end(); ++mvit) {
        Vertex *v = *mvit;
        glm::vec2 p = glm::vec2(float(v->point()[0]), float(v->point()[1]));
//        pos.push_back(p);

        if (p.x < minp.x) { minp.x = p.x;}
        if (p.y < minp.y) { minp.y = p.y;}
        if (p.x > maxp.x) { maxp.x = p.x;}
        if (p.y > maxp.y) { maxp.y = p.y;}

//        colors.push_back(glm::vec4(0, 0.4f, 0.7f, 1.0f));
    }

    center = (minp + maxp) / 2.0f;
    diameter = 2 * std::max(glm::length(minp-center), glm::length(maxp-center));

    for (MeshFaceIterator mfit(mesh); !mfit.end(); ++mfit) {
        Face *f = *mfit;
        f->setColor(ColorManager::COLOR_NORMAL_FACE);
        for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit) {
			
            pos.push_back(glm::vec2((*fvit)->point()[0], (*fvit)->point()[1]));
            colors.push_back(glm::vec4(ColorManager::COLOR_NORMAL_FACE[0], ColorManager::COLOR_NORMAL_FACE[1], ColorManager::COLOR_NORMAL_FACE[2], 1.0f));
        }
    }
    triangle_num = pos.size();

    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboVerticesID);
    glGenBuffers(1, &vboColorID);

    //TODO: clean the vboIndexID
//    glGenBuffers(1, &vboIndexID);

    GLsizei stride = sizeof(glm::vec2);
    glBindVertexArray(vaoID);

    glBindBuffer (GL_ARRAY_BUFFER, vboVerticesID);
    //pass triangle verteices to buffer object

    glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec2) * pos.size(), &pos[0], GL_STATIC_DRAW);
    //enable vertex attribute array for position
    glEnableVertexAttribArray(colorShader["vVertex"]);
    glVertexAttribPointer(colorShader["vVertex"], 2, GL_FLOAT, GL_FALSE,stride,0);

    glBindBuffer (GL_ARRAY_BUFFER, vboColorID);
    glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(colorShader["vColor"]);
    glVertexAttribPointer(colorShader["vColor"], 4, GL_FLOAT, GL_FALSE,4 * sizeof(GLfloat), 0);

}


void OnRender() {
	GLSLShader colorShader = ShaderManager::getShader("Color");
	GLSLShader wireframeShader = ShaderManager::getShader("WireFrame");

    glEnable(GL_BLEND);

    //    glm::mat4 T		= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -50));
//
//    glm::mat4 Rx	= glm::rotate(T,  rotation_x, glm::vec3(1.0f, 0.0f, 0.0f));
//    glm::mat4 Ry	= glm::rotate(Rx, rotation_y, glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 MV	= glm::rotate(Ry, rotation_z, glm::vec3(0.0f, 0.0f, 1.0f));

    const float width  = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;


    P = glm::mat4(1);
    MV = glm::translate(glm::mat4(1.0f),glm::vec3(-center.x, -center.y, 0.0));
    MV = glm::translate(MV,glm::vec3(translate_x, translate_y, 0.0));
    float aspect = (float)width / (float)height;
    P = glm::ortho(-aspect * diameter, aspect * diameter, -diameter, diameter, -1.0f, 1.0f);


    colorShader.Use();
    glUniformMatrix4fv(colorShader("MVP"), 1, GL_FALSE, glm::value_ptr(P*MV));
    MVP_inv = glm::inverse(P*MV);
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, triangle_num);
    glBindVertexArray(0); //unbind
    colorShader.UnUse();



    wireframeShader.Use();
    glUniformMatrix4fv(wireframeShader("MVP"), 1, GL_FALSE, glm::value_ptr(P*MV));
    glBindVertexArray(vaoID);
//    glDrawElements(GL_TRIANGLES, triangle_num, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, triangle_num);

    glBindVertexArray(0); //unbind
    wireframeShader.UnUse();

    glDisable(GL_BLEND);
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

    regionGrowing = new RegionGrowing();
    regionGrowing->initPatches(mesh, 50);

	GUI::init();
	GLFWwindow *window = GUI::getWindow();

    ShaderManager::init();
    prepareMeshData(mesh);
    constructKDTree(mesh);

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
        if (0 && show_another_window)
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
		if (0) {
			
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7, 0.7, 0.7, 0));
			const ImGuiWindowFlags layout_flags 
				= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoMove	| ImGuiWindowFlags_NoResize;

			ImGui::Begin("Mode", nullptr, ImVec2(200,20), 0.6, layout_flags);
			
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::TextColored(ImVec4(1, 0, 0, 1),MODE_STR[progStatus]);
			ImGui::End();
			ImGui::PopStyleColor();
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

        std::vector<glm::vec4> colors;
        for (MeshFaceIterator mfit(mesh); !mfit.end(); ++mfit) {
            Face *f = *mfit;
            for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit) {
				Point fcol = f->getColor();
                 colors.push_back(glm::vec4(fcol[0], fcol[1], fcol[2], 1.0f));
            }
        }

        glBindBuffer (GL_ARRAY_BUFFER, vboColorID);
        glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(), &colors[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(colorShader["vColor"]);
        glVertexAttribPointer(colorShader["vColor"], 4, GL_FLOAT, GL_FALSE,4 * sizeof(GLfloat), 0);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        OnRender();
        if (progStatus == PS_PICKING_SEED) {
            char buf[128];


            if (pickingFace) {
                ImGui::SetTooltip("Face %d", pickingFace->index());
            } else {
                ImGui::SetTooltip("     Picking    ");
            }

        }

        ImGui::Render();


        glfwSwapBuffers(window);
    }

    if (presults) {
        kd_res_free(presults );
    }

    kd_free( ptree );

    OnShutdown();
    UnLoadDrawingData();
    ImGui::Shutdown();
   
    delete actWindow;
    delete mesh;
    exit(EXIT_SUCCESS);
}