#version 400 core
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormalVertex;
uniform vec3 model_color;

out vec4 intercolor;
out vec4 position_world;
out vec4 normal;

void
main()
{
    gl_Position = vec4(vPosition.x,vPosition.y,0,1.0);
    intercolor = vec4(model_color,1.0);
}
