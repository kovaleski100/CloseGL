
#version 400 core

layout (location = 0) in vec4 NDC_coefficients;
layout (location = 1) in vec4 color_coefficients;

out vec4 intercolor;

void
main()
{
    gl_Position = NDC_coefficients;
    intercolor = color_coefficients;
}
