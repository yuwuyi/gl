cmake_minimum_required(VERSION 2.8.4)
project(gl)


#GLFW

add_subdirectory(glfw-3.0.4)
add_subdirectory(glew)
add_subdirectory(glm)



include_directories(glfw-3.0.4/include)
include_directories(glew/include)
include_directories(glm)

set(GLEW_INCLUDE_DIR "glew/include")

find_package( GLEW REQUIRED )
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(${GLEW_DEFINITIONS})

add_definitions(-DGLEW_STATIC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(SHADERS_DIR "shaders")
set(SHADERS_SRC ${SHADERS_DIR}/colorshader.frag ${SHADERS_DIR}/colorshader.vert ${SHADERS_DIR}/colorshader.geom
                ${SHADERS_DIR}/textureshader.frag ${SHADERS_DIR}/textureshader.vert)

set(MESHLIB_DIR "MeshLib_Core")
file(GLOB MESHLIB_SRC ${MESHLIB_DIR}/*)

set(IMGUI_DIR "imgui")
set(IMGUI_SRC ${IMGUI_DIR}/imconfig.h ${IMGUI_DIR}/imgui.h ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/stb_image.h ${IMGUI_DIR}/stb_textedit.h)

set(KDTREE_DIR "kdtree")
set(KDTREE_SRC ${KDTREE_DIR}/kdtree.h ${KDTREE_DIR}/kdtree.c)

set(GROWING_DIR "growing")
set(GROWING_SRC ${GROWING_DIR}/growing.h ${GROWING_DIR}/growing.cpp)

set(SOURCE_FILES main.cpp  GLSLShader.cpp GLSLShader.h glew/src/glew.c GUI.h GUI.cpp imgui_stuff.h imgui_stuff.cpp ActionWindow.cpp ActionWindow.h 
	ShaderManager.h ShaderManager.cpp
    ${SHADERS_SRC} ${IMGUI_SRC} ${MESHLIB_SRC} ${KDTREE_SRC} ${GROWING_SRC})

add_executable(gl ${SOURCE_FILES})

target_link_libraries(gl glfw ${GLFW_LIBRARIES})
target_link_libraries(gl glfw ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} imm32.lib)
