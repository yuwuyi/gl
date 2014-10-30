#version 330
 
in vec2 vUV;
in vec4 vColor;
in vec3 vVertex;

smooth out vec2 vTexCoord;
smooth out vec4 vSmoothColor;		//smooth colour to fragment shader


uniform mat4 MVP;
void main()
{
   vSmoothColor = vColor;

   vTexCoord = vUV;
   gl_Position = MVP*vec4(vVertex,1);
}
