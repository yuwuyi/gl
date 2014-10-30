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

GLFWwindow *window;
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

GLSLShader colorShader, textureShader, wireframeShader;

void barycentric(Face *f, const Point& point, float  (&lambda)[3]) {
     Point points [3] ;
    int i = 0;
    for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit, ++i) {
        points[i] = (*fvit)->point();
    }

    double x = point[0], y = point[1];
    double x1 = points[0][0], y1 = points[0][1];
    double x2 = points[1][0], y2 = points[1][1];
    double x3 = points[2][0], y3 = points[2][1];

    double dimo = ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
    lambda[0] = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / dimo;
    lambda[1] = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / dimo;
    lambda[2] = 1.0 - lambda[0] - lambda[1];

}


Face *locate(Vertex *vertex, const Point& point) {
    Face *inFace = nullptr;
    for (VertexFaceIterator vfit(vertex); !vfit.end(); ++vfit) {
        inFace = *vfit;
        float lambda[3];
        barycentric(inFace, point, lambda);

        for (int i = 0; i < 3; ++i) {
            if (lambda[i] < 0 || lambda[i] > 1) {
                inFace = nullptr;
                break;
            }
        }

        if (inFace) {
            break;
        }
    }

    return inFace;
}

static bool isPicking = false;
static float translate_x = 0, translate_y = 0;

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    const float window_width = ImGui::GetIO().DisplaySize.x;

    const float window_height = ImGui::GetIO().DisplaySize.y;


    if (action == GLFW_PRESS && button >= 0 && button < 2)
        mousePressed[button] = true;


    if (actWindow->isShown()) {
        return;
    }
    double xpos = 0, ypos = 0;
    glfwGetCursorPos(window, &xpos, &ypos);

    if(progStatus == PS_PICKING_SEED && !ImGui::IsMouseHoveringAnyWindow() && button==GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {

        //Point Location
    }
}

static void glfw_mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
    if (actWindow->isShown()) {
        return;
    }
    const float window_width = ImGui::GetIO().DisplaySize.x;

    const float window_height = ImGui::GetIO().DisplaySize.y;


    if(progStatus == PS_PICKING_SEED && !ImGui::IsMouseHoveringAnyWindow())
    {

        double xx, yy;

        xx = -(window_width/window_height * diameter) + xpos/window_width * 2 * (window_width / window_height * diameter);
        yy = ( diameter) - ypos/window_height * 2 * diameter;

//        std::cout << "xx, yy" << xx + center.x + translate_x<< " " << yy  + center.y + translate_y<< "\n";
        double pos[3] = {xx + center.x - translate_x, yy  + center.y - translate_y, 0};
        presults = kd_nearest(ptree, pos);

        Vertex *v = (Vertex*)kd_res_item( presults, pos );

        //Point Location
        Face *prevPickingFace = pickingFace;
        pickingFace = locate(v, Point(xx + center.x,  yy  + center.y, 0));
        if (pickingFace != prevPickingFace) {
            if (pickingFace) {
                pickingFace->color = ColorManager::COLOR_PICKED_FACE;
            }
            if (prevPickingFace) {
                prevPickingFace->color = ColorManager::COLOR_NORMAL_FACE;
            }

        }
    }

}




static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = (yoffset != 0.0f) ? yoffset > 0.0f ? 1 : - 1 : 0;           // Mouse wheel: -1,0,+1


    if (actWindow->isShown()) {
        return;
    }

    if (ImGui::IsMouseHoveringAnyWindow()) {
        return;
    }

    if (io.MouseWheel == -1) {
        //regionGrowing->grow();

        diameter *= 1.1;

    } else if (io.MouseWheel == 1) {
        diameter *= 0.9;
    }

}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    io.KeyCtrl = (mods & GLFW_MOD_CONTROL) != 0;
    io.KeyShift = (mods & GLFW_MOD_SHIFT) != 0;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        actWindow->setShow(!actWindow->isShown());
    }

    if (ImGui::IsMouseHoveringAnyWindow()) {
        return;
    }


    if (actWindow->isShown()) {
        return;
    }

    int factor = 1;
    if (mods & GLFW_MOD_ALT) {
        factor = 5;
    }

    if (key == GLFW_KEY_W) {
        translate_y -= diameter / 100 * factor;
    } else if (key == GLFW_KEY_S) {
        translate_y += diameter / 100 * factor;
    } else if (key == GLFW_KEY_A) {
        translate_x -= diameter / 100 * factor;
    } else if (key == GLFW_KEY_D) {
        translate_x += diameter / 100 * factor;
    }
}

static void glfw_char_callback(GLFWwindow* window, unsigned int c)
{
    if (c > 0 && c < 0x10000)
        ImGui::GetIO().AddInputCharacter((unsigned short)c);
}

//Here I'm going to try using the ImDrawVert..
GLuint vaoID, vboColorID, vboIndexID;
GLuint vboVerticesID;
GLint triangle_num;

void constructKDTree(Mesh *mesh) {
    ptree = kd_create( 3 );

    /* add some random nodes to the tree (assert nodes are successfully inserted) */
    for(MeshVertexIterator mvit(mesh); !mvit.end(); ++mvit) {
        Vertex *v = *mvit;
        assert( 0 == kd_insert3( ptree, v->point()[0], v->point()[1], v->point()[2], v ) );
    }
}


static void prepareMeshData(Mesh *mesh) {
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
        f->color = ColorManager::COLOR_NORMAL_FACE;
        for (FaceVertexIterator fvit(f); !fvit.end(); ++fvit) {
//            indices.push_back((*fvit)->index());
            pos.push_back(glm::vec2((*fvit)->point()[0], (*fvit)->point()[1]));
            colors.push_back(glm::vec4(f->color[0], f->color[1], f->color[2], 1.0f));
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




void initShader() {
    colorShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/colorshader.vert");
    colorShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/colorshader.frag");

    //compile and link shader
    colorShader.CreateAndLinkProgram();
    colorShader.Use();
    //add attributes and uniforms
    colorShader.AddAttribute("vVertex");
    colorShader.AddAttribute("vColor");
    colorShader.AddAttribute("vUV");
    colorShader.AddUniform("MVP");
    colorShader.AddUniform("textureMap");
    glUniform1i(colorShader("textureMap"),0);
    colorShader.UnUse();



    textureShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/textureshader.vert");
    textureShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/textureshader.frag");

    //compile and link shader
    textureShader.CreateAndLinkProgram();
    textureShader.Use();
    //add attributes and uniforms
    textureShader.AddAttribute("vVertex");
    textureShader.AddAttribute("vColor");
    textureShader.AddAttribute("vUV");
    textureShader.AddUniform("MVP");
    textureShader.AddUniform("textureMap");
    glUniform1i(textureShader("textureMap"),0);
    textureShader.UnUse();

    wireframeShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/wireframeshader.vert");
    wireframeShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/wireframeshader.geom");
    wireframeShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/wireframeshader.frag");

    //compile and link shader
    wireframeShader.CreateAndLinkProgram();
    wireframeShader.Use();
    //add attributes and uniforms
    wireframeShader.AddAttribute("vVertex");
    wireframeShader.AddAttribute("vColor");
    wireframeShader.AddAttribute("vUV");
    wireframeShader.AddUniform("MVP");
    wireframeShader.AddUniform("textureMap");
    glUniform1i(wireframeShader("textureMap"),0);
    wireframeShader.UnUse();

}

static void OnShutdown() {
    //Destroy shader
    colorShader.DeleteShaderProgram();
    textureShader.DeleteShaderProgram();
    //Destroy vao and vbo
    glDeleteBuffers(1, &vboVerticesID);
    glDeleteVertexArrays(1, &vaoID);
    cout<<"Shutdown successfull"<<endl;
}

enum BUTTON_CMD {
    BTN_PICK_SEEDS,
};



static bool button_pressed[8] = {false, false, false, false, false, false, false, false};

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " input.m\n";
        exit(-1);
    }

    Mesh *mesh = new Mesh;
    mesh->readMFile(argv[1]);

    regionGrowing = new RegionGrowing();
    regionGrowing->initPatches(mesh, 50);

    if (!glfwInit())
        exit(EXIT_FAILURE);



    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);


    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(1080, 768, "Simple example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window, glfw_mouse_move_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetCharCallback(window, glfw_char_callback);


    //initial glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << glGetError() << "\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    err = glGetError(); //this is to ignore INVALID ENUM error 1282

    initShader();
    prepareMeshData(mesh);
    constructKDTree(mesh);

    InitImGui();

    actWindow = new ActionWindow(window);

    const float window_width = ImGui::GetIO().DisplaySize.x;

    const float window_height = ImGui::GetIO().DisplaySize.y;

    while (!glfwWindowShouldClose(window))
    {

        ImGuiIO& io = ImGui::GetIO();
        mousePressed[0] = mousePressed[1] = false;
        io.MouseWheel = 0;
        glfwPollEvents();
        UpdateImGui();


        static bool show_test_window = false;
        static bool show_another_window = false;

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        if(0) {
            static float f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            show_test_window ^= ImGui::Button("Test Window");
//            show_another_window ^= ImGui::Button("Another Window");
            ImGui::Checkbox("Picking", &isPicking);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (0 && show_another_window)
         {
            ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (1 || show_test_window)
        {
            ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));        // Normally user code doesn't need/want to call this, because positions are saved in .ini file. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowTestWindow(&show_test_window);
        }

        {
            if (actWindow->isShown()) {

               actWindow->render();
            }
        }


        if (button_pressed[0]) {
            regionGrowing->grow();
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
                 colors.push_back(glm::vec4(f->color[0], f->color[1], f->color[2], 1.0f));
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
    glfwDestroyWindow(window);
    glfwTerminate();
    delete actWindow;
    delete mesh;
    exit(EXIT_SUCCESS);
}