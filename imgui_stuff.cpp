#ifdef _MSC_VER
#pragma warning (disable: 4996)         // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#include <Windows.h>
#include <Imm.h>
#endif
#include "GLSLShader.h"
#include <vector>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderManager.h"

#include <iostream>
#include <fstream>


#include "imgui_stuff.h"
#include "imgui/imgui.h"

#define STB_IMAGE_IMPLEMENTATION

#include "imgui/stb_image.h"                  // for .png loading
#include "imgui/stb_textedit.h"                  // for .png loading

#ifdef _MSC_VER
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

extern GLFWwindow *window;


static GLuint fontTex;
bool mousePressed[2] = { false, false };
static ImVec2 mousePosScale(1.0f, 1.0f);
GLuint imgui_vaoID;
GLuint imgui_vboVerticesID;
GLuint imgui_vboColorID;

GLuint imgui_vernumber;
GLuint vboTexCoordID;

static const char* ImImpl_GetClipboardTextFn()
{
    return glfwGetClipboardString(window);
}

static void ImImpl_SetClipboardTextFn(const char* text)
{
    glfwSetClipboardString(window, text);
}

#ifdef _MSC_VER
// Notify OS Input Method Editor of text input position (e.g. when using Japanese/Chinese inputs, otherwise this isn't needed)
static void ImImpl_ImeSetInputScreenPosFn(int x, int y)
{
	HWND hwnd = glfwGetWin32Window(window);
	if (HIMC himc = ImmGetContext(hwnd))
	{
		COMPOSITIONFORM cf;
		cf.ptCurrentPos.x = x;
		cf.ptCurrentPos.y = y;
		cf.dwStyle = CFS_FORCE_POSITION;
		ImmSetCompositionWindow(himc, &cf);
	}
}


#endif
void InitImGui()
{
    int w, h;
    int fb_w, fb_h;
    glfwGetWindowSize(window, &w, &h);
    glfwGetFramebufferSize(window, &fb_w, &fb_h);
    mousePosScale.x = (float)fb_w / w;                  // Some screens e.g. Retina display have framebuffer size != from window size, and mouse inputs are given in window/screen coordinates.
    mousePosScale.y = (float)fb_h / h;

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)fb_w, (float)fb_h);  // Display size, in pixels. For clamping windows positions.
    io.DeltaTime = 1.0f/60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
    io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;             // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.RenderDrawListsFn = ImImpl_RenderDrawLists;
    io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
    io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
#ifdef _MSC_VER
    io.ImeSetInputScreenPosFn = ImImpl_ImeSetInputScreenPosFn;
#endif

//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);   //Modulate RGB with RGB
    // Load font texture
    glGenTextures(1, &fontTex);
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#if 0
    // Default font (embedded in code)
    const void* png_data;
    unsigned int png_size;
    ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
    int tex_x, tex_y, tex_comp;
    void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
    IM_ASSERT(tex_data != NULL);
#else
    // Custom font from filesystem
    io.Font = new ImBitmapFont();
    io.Font->LoadFromFile("extra_fonts/mplus-2m-medium_18.fnt");
    IM_ASSERT(io.Font->IsLoaded());

    int tex_x, tex_y, tex_comp;
    void* tex_data = stbi_load("extra_fonts/mplus-2m-medium_18.png", &tex_x, &tex_y, &tex_comp, 0);
    IM_ASSERT(tex_data != NULL);

    // Automatically find white pixel from the texture we just loaded
    // (io.FontTexUvForWhite needs to contains UV coordinates pointing to a white pixel in order to render solid objects)
    for (int tex_data_off = 0; tex_data_off < tex_x*tex_y; tex_data_off++)
        if (((unsigned int*)tex_data)[tex_data_off] == 0xffffffff)
        {
            io.FontTexUvForWhite = ImVec2((float)(tex_data_off % tex_x)/(tex_x), (float)(tex_data_off / tex_x)/(tex_y));
            break;
        }
#endif

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    stbi_image_free(tex_data);

    glGenVertexArrays(1, &imgui_vaoID);
    glGenBuffers(1, &imgui_vboVerticesID);
    glGenBuffers (1, &vboTexCoordID);
    glGenBuffers(1, &imgui_vboColorID);
}

void UpdateImGui()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup timestep
    static double time = 0.0f;
    const double current_time =  glfwGetTime();
    io.DeltaTime = (float)(current_time - time);
    time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    io.MousePos = ImVec2((float)mouse_x * mousePosScale.x, (float)mouse_y * mousePosScale.y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    io.MouseDown[0] = mousePressed[0] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = mousePressed[1] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != 0;

    // Start the frame
    ImGui::NewFrame();
}



void RenderDrawingLists(ImDrawList** const cmd_lists, int cmd_lists_count) {
	GLSLShader textureShader = ShaderManager::getShader("Texture");

    glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    textureShader.Use();
    //pass the shader uniform
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;

//    glm::mat4 T		= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -50));
//    glm::mat4 Rx	= glm::rotate(T,  rotation_x, glm::vec3(1.0f, 0.0f, 0.0f));
//    glm::mat4 Ry	= glm::rotate(Rx, rotation_y, glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 MV	= glm::rotate(Ry, rotation_z, glm::vec3(0.0f, 0.0f, 1.0f));
//
    glm::mat4 MV = glm::mat4(1);
    glm::mat4 P = glm::ortho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
    //P = glm::perspective(0.0f, (float)width / (float)height, -1.0f, 1.0f);
    glUniformMatrix4fv(textureShader("MVP"), 1, GL_FALSE, glm::value_ptr(P*MV));
//    //draw triangle



    //load data


    glBindVertexArray(imgui_vaoID);
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
       // glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
       // glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
       // glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));

        glBindBuffer (GL_ARRAY_BUFFER, imgui_vboVerticesID);
        glBufferData (GL_ARRAY_BUFFER,cmd_list->vtx_buffer.size() * sizeof(ImDrawVert), vtx_buffer, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(textureShader["vVertex"]);
        glVertexAttribPointer(textureShader["vVertex"], 2, GL_FLOAT, GL_FALSE,sizeof(ImDrawVert),0);

        glBindBuffer (GL_ARRAY_BUFFER, vboTexCoordID);
        glBufferData (GL_ARRAY_BUFFER, cmd_list->vtx_buffer.size() * sizeof(ImDrawVert), vtx_buffer, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(textureShader["vUV"]);
        glVertexAttribPointer (textureShader["vUV"], 2, GL_FLOAT, GL_FALSE,sizeof(ImDrawVert), (GLvoid*) (offsetof(ImDrawVert, uv)));

        glBindBuffer (GL_ARRAY_BUFFER, imgui_vboColorID);
        glBufferData (GL_ARRAY_BUFFER, cmd_list->vtx_buffer.size() * sizeof(ImDrawVert), vtx_buffer, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(textureShader["vColor"]);
        glVertexAttribPointer(textureShader["vColor"], 4, GL_UNSIGNED_BYTE, GL_TRUE,sizeof(ImDrawVert),  (GLvoid*) (offsetof(ImDrawVert, col)));
//


        int vtx_offset = 0;
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
        {
            glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));

            //glBindVertexArray(imgui_vaoID);
            glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            vtx_offset += pcmd->vtx_count;
        }
    }


    glBindVertexArray(0); //unbind

    textureShader.UnUse();
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
}

void UnLoadDrawingData() {
    //Destroy vao and vbo
    glDeleteBuffers(1, &imgui_vaoID);
    glDeleteVertexArrays(1, &imgui_vboVerticesID);
    glDeleteBuffers(1, &vboTexCoordID);

}


void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{


    if (cmd_lists_count == 0)
        return;


    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

   glBindTexture(GL_TEXTURE_2D, fontTex);
   glEnable(GL_TEXTURE_2D);

//   ImImpl_LoadDrawingData(cmd_lists, cmd_lists_count);
    RenderDrawingLists(cmd_lists, cmd_lists_count);
    glDisable(GL_SCISSOR_TEST);

}


