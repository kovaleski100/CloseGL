//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw_gl3.h"
#include "Myobj.cpp"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <X11/Xlib.h>
#include <math.h>

#define BUFFER_OFFSET(a) ((void *)(a))

using namespace std;

using namespace ImGui;

typedef struct
{
    GLenum type;
    const char *filename;
    GLuint shader;
} ShaderInfo;

enum VAO_IDs
{
    Triangles,
    NumVAOs
};

enum Buffer_IDs
{
    VtxBuffer = 0,
    NormBuffer = 1,
    NumBuffers = 2
};
enum Attrib_IDs
{
    vPosition = 0,
    vNormalVertex = 1
};

GLuint Buffers[NumBuffers];
GLuint NumVertices;

GLint model_uniform;
GLint view_uniform;
GLint projection_uniform;

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 ModelViewIT;

float g_CameraTheta = 0.0f;    // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;      // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

float pos = 10.0f;

glm::vec3 camera_position_c = glm::vec3(0, 0, 0);
glm::vec3 camera_position_O = glm::vec3(0, 0, 0);
glm::vec3 camera_lookat_l = glm::vec3(0, 0.0f, 0.0f);
glm::vec3 camera_up_vector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_view_vector;

double g_LastCursorPosX, g_LastCursorPosY;
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;  // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
if (key == GLFW_KEY_W)
    {
        float inc = 0.05f;
        camera_position_c += inc* camera_lookat_l;
    }
    if (key == GLFW_KEY_S)
    {
        float inc = 0.05f;
        camera_position_c -= inc* camera_lookat_l;
    }
    if (key == GLFW_KEY_A)
    {
        float inc = 0.05f;
        camera_position_c -= inc* glm::cross(camera_lookat_l,camera_up_vector);
    }
    if (key == GLFW_KEY_D)
    {
        float inc = 0.05f;
        camera_position_c += inc* glm::cross(camera_lookat_l,camera_up_vector);
    }
}

static const GLchar *
ReadShader(const char *filename)
{
    FILE *infile = fopen(filename, "rb");

    if (!infile)
    {
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
        return NULL;
    }

    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    GLchar *source = new GLchar[len + 1];

    fread(source, 1, len, infile);
    fclose(infile);

    source[len] = 0;

    return const_cast<const GLchar *>(source);
}

GLuint LoadShaders(ShaderInfo *shaders)
{
    if (shaders == NULL)
    {
        return 0;
    }

    GLuint program = glCreateProgram();

    ShaderInfo *entry = shaders;
    while (entry->type != GL_NONE)
    {
        GLuint shader = glCreateShader(entry->type);

        entry->shader = shader;

        const GLchar *source = ReadShader(entry->filename);
        if (source == NULL)
        {
            for (entry = shaders; entry->type != GL_NONE; ++entry)
            {
                glDeleteShader(entry->shader);
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource(shader, 1, &source, NULL);
        delete[] source;

        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLsizei len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            GLchar *log = new GLchar[len + 1];
            glGetShaderInfoLog(shader, len, &len, log);
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete[] log;

            return 0;
        }

        glAttachShader(program, shader);

        ++entry;
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        GLchar *log = new GLchar[len + 1];
        glGetProgramInfoLog(program, len, &len, log);
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete[] log;

        for (entry = shaders; entry->type != GL_NONE; ++entry)
        {
            glDeleteShader(entry->shader);
            entry->shader = 0;
        }

        return 0;
    }

    return program;
}

GLuint BuildTriangles(MyObj *obj_load)
{

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //
    //   Create buffers for vertices and normals
    //
    glCreateBuffers(NumBuffers, Buffers);
    //
    //   Bind the vextex buffer, allocate storage for it, and load the vertex coordinates. Then,
    //   bind the normal buffer, allocate storage and load normal coordinates
    //
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[VtxBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, obj_load->NumTris * 9 * sizeof(GL_FLOAT), obj_load->Vert, GL_DYNAMIC_STORAGE_BIT); // Vert is the array with coordinates of
                                                                                                                        // the vertices created after reading the file
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);
    //
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[NormBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, obj_load->NumTris * 9 * sizeof(GL_FLOAT),
                    obj_load->Vert_Normal, GL_DYNAMIC_STORAGE_BIT); // Vert_Normal is the
                                                                    // array with vertex normals  created after reading the file

    glVertexAttribPointer(vNormalVertex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vNormalVertex);

    //
    //  set the number of vertices, which will be used in the openglDisplay function for calling
    //  glDrawArrays
    //
    glBindVertexArray(0);
    NumVertices = obj_load->NumTris * 3;

    return VAO;
}

void set_windowPos(GLFWwindow *interface, GLFWwindow *window)
{
    int width;
    int height;

    glfwGetWindowPos(window, &width, &height);
    glfwSetWindowPos(interface, width + 850, height);
}

int main(int argc, char **argv)
{
    glfwInit();

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    GLFWwindow *window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);
    glfwSetKeyCallback(window, KeyCallback);


    if (window == NULL)
        return 1;

    GLFWwindow *interface = glfwCreateWindow(300, 400, "Options", NULL, NULL);

    set_windowPos(interface, window);

    glfwMakeContextCurrent(window);
    glewInit();

    glfwMakeContextCurrent(interface);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(interface, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    //ImVec4 clear_color = ImVec4(0.6f, 0.3f, 0.6f, 1.00f);

    glfwMakeContextCurrent(window);

    MyObj *obj_load = new MyObj();

    char *filename = "cow_up.in",filenameAux[20] = "";
    //char filename[20] = "cube.in", filenameAux[20] = "";

    obj_load->load_obj(filename);

    //int NumVertices_Obj = BuildTriangles(obj_load);

    ShaderInfo shaders[] =
        {
            {GL_VERTEX_SHADER, "../../vert.glsl"},
            {GL_FRAGMENT_SHADER, "../../frag.glsl"},
            {GL_NONE, NULL}};

    GLuint program = LoadShaders(shaders);

    model_uniform = glGetUniformLocation(program, "model");           // Variável da matriz "model"
    view_uniform = glGetUniformLocation(program, "view");             // Variável da matriz "view" em shader_vertex.glsl
    projection_uniform = glGetUniformLocation(program, "projection"); // Variável da matriz "projection" em shader_vertex.glsl

    //glVertexAttribPointer(vPosition, 2, GL_FLOAT,
    //                      GL_FALSE, 0, BUFFER_OFFSET(0));
    //glEnableVertexAttribArray(vPosition);
    GLuint VAO = BuildTriangles(obj_load);

    camera_position_c.x = obj_load->bbox_middle.x;
    camera_position_c.y = obj_load->bbox_middle.y;
    camera_position_c.z = 2*std::max((obj_load->bbox_max.y - obj_load->bbox_min.y)*(1.0/tan(45)),(obj_load->bbox_max.x - obj_load->bbox_min.x)*(1.0/tan(45)));

    camera_position_O = camera_position_c;

    int lookAt = 0;
    int axis = 0;
    float near = 0.1;
    float far = 3000;
    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(interface))
    {
        static const float black[] = {1.0f, 1.0f, 1.0f, 0.0f};

        glClearBufferfv(GL_COLOR, 0, black);

        glUseProgram(program);
        glfwMakeContextCurrent(window);
        
        camera_lookat_l = obj_load->bbox_middle - camera_position_c;
        //camera_lookat_l = glm::normalize(camera_lookat_l);

        /*
        switch(axis)
        {
            case 0:

                glm::translate(glm::mat4(1.0f),camera_position_c);
            break;
        }
        */
        View = glm::lookAt(camera_position_c, obj_load->bbox_middle, camera_up_vector);

        //printf("min posx = %f, posy = %f, posz = %f\n", obj_load->bbox_min.x, obj_load->bbox_min.y, obj_load->bbox_min.z);
        //printf("max posx = %f, posy = %f, posz = %f\n", obj_load->bbox_max.x, obj_load->bbox_max.y, obj_load->bbox_max.z);
        //glBindVertexArray(vertex_array_object_id);

        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);
        printf("max %f\n", sqrt(pow(camera_position_c.x-obj_load->bbox_middle.x,2) + pow(camera_position_c.y-obj_load->bbox_middle.y,2) + pow(camera_position_c.z-obj_load->bbox_middle.z,2)));
        //printf("min posx = %f, posy = %f, posz = %f\nf", obj_load->bbox_min.x, obj_load->bbox_min.y, obj_load->bbox_min.z);
        //printf("Tamx = %f, Tamy = %f, Tamz = %f\n", scalax, scalay, scalaz);

        glfwSwapBuffers(window);
        glfwPollEvents();
        Projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, near, far);

        glm::mat4 model = glm::mat4(1.0);

        ModelViewIT = glm::transpose(glm::inverse(View * model));

        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(Projection));
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glfwMakeContextCurrent(interface);

        //static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("My window"); // create window
        ImGui::InputText("obj", filenameAux, 20);
        ImGui::NewLine();
        ImGui::RadioButton("Look At", &lookAt,1 );
        ImGui::NewLine();
        ImGui::RadioButton("Free", &lookAt,0 );
        ImGui::Text("clipping planes");
        ImGui::NewLine();
        ImGui::InputFloat("Near",&near);
        ImGui::NewLine();
        ImGui::InputFloat("Far",&far);
        ImGui::NewLine();
        if (ImGui::Button("Reset camera position"))
        {
            camera_position_c = camera_position_O;
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(interface);

        glClearBufferfv(GL_COLOR, 0, black);

        glfwMakeContextCurrent(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
}