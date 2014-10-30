#version 330
smooth in vec2 vTexCoord;
out vec4 vFragColor;
smooth in vec4 vSmoothColor;		//interpolated colour to fragment shader

uniform sampler2D textureMap;

void main(void)
{
   vFragColor = vec4(0, 0, 0, 1);
}
