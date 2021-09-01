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
#define X 0
#define Y 1
#define Z 2

using namespace std;

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
GLint model_color;

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 ModelViewIT;

float g_CameraTheta = 0.0f;    // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;      // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;
float MouseDX = 0.0f;
float MouseDY = 0.0f;
float pos = 10.0f;

float pitch, yaw, roll;

int lookAt;
int axis;
int translate = 1;

MyObj *obj_load = new MyObj();

glm::vec3 camera_position_c = glm::vec3(0, 0, 0);
glm::vec3 camera_position_O = glm::vec3(0, 0, 0);
glm::vec3 camera_lookat_l_O = glm::vec3(0, 0.0f, 0.0f);
glm::vec3 camera_up_vector_O = glm::vec3(0, 0, 0);
glm::vec3 camera_lookat_l = glm::vec3(0, 0.0f, 0.0f);
glm::vec3 camera_up_vector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_side_vector;
glm::vec3 Color_model = glm::vec3(1, 0, 0);
glm::vec3 direction;

double g_LastCursorPosX, g_LastCursorPosY;
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;  // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{

    float inc = 10.0f;
    if (key == GLFW_KEY_W)
    {
        camera_position_c += inc * camera_lookat_l;
    }
    if (key == GLFW_KEY_S)
    {
        camera_position_c -= inc * camera_lookat_l;
    }
    if (key == GLFW_KEY_A)
    {
        camera_position_c -= inc * glm::normalize(glm::cross(camera_lookat_l, camera_up_vector));
    }
    if (key == GLFW_KEY_D)
    {
        camera_position_c += inc * glm::normalize(glm::cross(camera_lookat_l, camera_up_vector));
    }
    if(key == GLFW_KEY_Q)
    {
        camera_position_c += inc * camera_up_vector;
    }
    if(key == GLFW_KEY_E)
    {
        camera_position_c -= inc * camera_up_vector;
    }
    camera_lookat_l = glm::normalize(obj_load->bbox_middle - camera_position_c);
    camera_up_vector = -glm::normalize(glm::cross(camera_lookat_l, glm::cross(camera_lookat_l,camera_up_vector)));
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da câmera com os deslocamentos

        float sens = 0.2;

        yaw = sens * dx;
        pitch = sens * dy;

        // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f / 2;
        float phimin = -phimax;

        if (!translate)
        {
            if (axis == X)
            {
                direction.x = camera_lookat_l.x;
                direction.y = camera_lookat_l.y * glm::cos(glm::radians(pitch)) - camera_lookat_l.z * glm::sin(glm::radians(pitch));
                direction.z = camera_lookat_l.y * glm::sin(glm::radians(pitch)) + camera_lookat_l.z * glm::cos(glm::radians(pitch));
            }
            if (axis == Y)
            {
                direction.x = camera_lookat_l.x * glm::cos(glm::radians(yaw)) - camera_lookat_l.z * glm::sin(glm::radians(yaw));

                direction.y = camera_lookat_l.y;

                direction.z = camera_lookat_l.x * glm::sin(glm::radians(yaw)) + camera_lookat_l.z * glm::cos(glm::radians(yaw));
            }
            if (axis == Z)
            {
                glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-pitch), camera_lookat_l);
                camera_up_vector = glm::mat3(roll_mat) * camera_up_vector;
            }
        }
        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos parâmetros da antebraço com os deslocamentos
        g_TorsoPositionX += 0.01f * dx;
        g_TorsoPositionY -= 0.01f * dy;

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
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
    
    GLFWwindow *window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);

    if (window == NULL)
        return 1;

    GLFWwindow *interface = glfwCreateWindow(450, 750, "Options", NULL, NULL);

    set_windowPos(interface, window);

    glfwMakeContextCurrent(window);
    glewInit();

    glfwMakeContextCurrent(interface);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(interface, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    char *filename = "cow_up.in", filenameAux[20] = "";

    obj_load->load_obj(filename);

    ShaderInfo shaders[] =
        {
            {GL_VERTEX_SHADER, "../../vert.glsl"},
            {GL_FRAGMENT_SHADER, "../../frag.glsl"},
            {GL_NONE, NULL}};

    GLuint program = LoadShaders(shaders);

    model_uniform = glGetUniformLocation(program, "model");           // Variável da matriz "model"
    view_uniform = glGetUniformLocation(program, "view");             // Variável da matriz "view" em shader_vertex.glsl
    projection_uniform = glGetUniformLocation(program, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    model_color = glGetUniformLocation(program, "model_color");

    GLuint VAO = BuildTriangles(obj_load);

    camera_position_c.x = obj_load->bbox_middle.x;
    camera_position_c.y = obj_load->bbox_middle.y;
    camera_position_c.z = 2 * std::max((obj_load->bbox_max.y - obj_load->bbox_min.y) * (1.0 / tan(45)), (obj_load->bbox_max.x - obj_load->bbox_min.x) * (1.0 / tan(45)));

    camera_position_O = camera_position_c;
    camera_lookat_l = glm::normalize(obj_load->bbox_middle - camera_position_c);
    camera_lookat_l_O = camera_lookat_l;
    camera_up_vector_O = camera_up_vector;

    camera_side_vector = glm::normalize(glm::cross(camera_lookat_l, camera_up_vector));

    float near = 0.1;
    float far = 3000;
    int render_mode = 0;
    int ccw = 1;
    int backFaceCull = 0;
    float g_ScreenRatio = 0.0f;
    float vfov = 60;
    float hfov = (vfov * 16) / 9;
    View = glm::lookAt(camera_position_c, obj_load->bbox_middle, camera_up_vector);
    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(interface))
    {
        static const float black[] = {1.0f, 1.0f, 1.0f, 0.0f};
        glDepthMask(GL_FALSE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, black);

        switch (render_mode)
        {
        case 0:
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
            break;

        case 1:
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
            break;
        default:
            glPolygonMode(GL_FRONT, GL_POINT);
            glPolygonMode(GL_BACK, GL_POINT);
            break;
        }

        glUseProgram(program);
        glfwMakeContextCurrent(window);
        if (ccw)
        {
            glFrontFace(GL_CCW);
        }
        else
        {
            glFrontFace(GL_CW);
        }
        if (backFaceCull)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        if (lookAt)
        {
            if (translate)
            {

                View = glm::lookAt(camera_position_c, obj_load->bbox_middle, camera_up_vector);
                camera_lookat_l = glm::normalize(obj_load->bbox_middle - camera_position_c);
                camera_side_vector = glm::normalize(glm::cross(camera_lookat_l, camera_up_vector));

            }
        }
        else
        {
            if (translate)
            {
                View = glm::lookAt(camera_position_c, camera_lookat_l + camera_position_c, camera_up_vector);
                direction = camera_lookat_l;
            }
            else
            {
                camera_lookat_l = direction;
                View = glm::lookAt(camera_position_c, camera_lookat_l + camera_position_c, camera_up_vector);
            }
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
        g_ScreenRatio = tan(glm::radians(hfov / 16)) / tan(glm::radians(vfov / 9));
        Projection = glm::perspective(glm::radians(vfov), g_ScreenRatio, near, far);

        glm::mat4 model = glm::mat4(1.0);

        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(Projection));
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(model_color, 1, glm::value_ptr(Color_model));

        glfwMakeContextCurrent(interface);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("My window"); // create window

        ImGui::Text("Application framerate (%.1f FPS)", ImGui::GetIO().Framerate);
        ImGui::InputText("obj", filenameAux, 20);
        ImGui::NewLine();
        ImGui::RadioButton("Look At", &lookAt, 1);
        ImGui::NewLine();
        ImGui::RadioButton("Free", &lookAt, 0);
        ImGui::NewLine();
        ImGui::Text("clipping planes");
        ImGui::NewLine();
        ImGui::InputFloat("Near", &near);
        ImGui::NewLine();
        ImGui::InputFloat("Far", &far);
        ImGui::NewLine();
        ImGui::InputFloat("vfov", &vfov);
        ImGui::NewLine();
        ImGui::InputFloat("hfov", &hfov);
        ImGui::NewLine();
        ImGui::RadioButton("Translate", &translate, 1);
        ImGui::NewLine();
        ImGui::RadioButton("Rotate", &translate, 0);
        ImGui::NewLine();
        ImGui::RadioButton("X", &axis, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Y", &axis, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Z", &axis, 2);
        ImGui::NewLine();
        ImGui::Text("Render Mode");
        ImGui::NewLine();
        ImGui::RadioButton("Fill", &render_mode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Line", &render_mode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Point", &render_mode, 2);
        ImGui::NewLine();
        ImGui::Text("Render Type");
        ImGui::NewLine();
        ImGui::RadioButton("CCW", &ccw, 1);
        ImGui::SameLine();
        ImGui::RadioButton("cw", &ccw, 0);
        ImGui::SameLine();
        ImGui::NewLine();
        ImGui::Text("backFace Culling");
        ImGui::NewLine();
        ImGui::RadioButton("on", &backFaceCull, 1);
        ImGui::SameLine();
        ImGui::RadioButton("off", &backFaceCull, 0);
        ImGui::NewLine();
        ImGui::ColorEdit3("Color", (float *)&Color_model);
        ImGui::NewLine();
        if (ImGui::Button("Reset camera position"))
        {
            camera_position_c.x = obj_load->bbox_middle.x;
            camera_position_c.y = obj_load->bbox_middle.y;
            camera_position_c.z = 2 * std::max((obj_load->bbox_max.y - obj_load->bbox_min.y) * (1.0 / tan(45)), (obj_load->bbox_max.x - obj_load->bbox_min.x) * (1.0 / tan(45)));
            camera_lookat_l = camera_lookat_l_O;
            direction = camera_lookat_l_O;
            camera_up_vector = camera_up_vector_O;
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