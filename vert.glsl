
#version 400 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormalVertex;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 intercolor;

void
main()
{
    gl_Position = projection * view * model * vPosition;
    intercolor = vec4(1,0,0,1);
}
