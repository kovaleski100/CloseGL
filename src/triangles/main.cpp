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
#include "matrices.cpp"

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
GLint model_color;
GLint view_uniform;
GLint projection_uniform;

GLint model_uniform_CL;
GLint model_color_CL;
GLint view_uniform_CL;
GLint projection_uniform_CL;

GLint phong_uniform;
GLint vertex_type_uniform;
GLuint no_shade_index;

GLuint no_shade_index_F;

GLint shading_uniform;
GLuint gourAD_shade_index;
GLuint gourADS_shade_index;
GLuint phong_shade_index;

glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 ModelViewIT;

glm::mat4 ProjectionGL;
glm::mat4 ViewGL;
glm::mat4 ModelGL;

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
int shading_index = 3;
int phong = 1;
float g_ScreenRatio = 0.0f;

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

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slides 33-44 do documento "Aula_07_Transformacoes_Geometricas_3D.pdf").
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slide 227 do documento "Aula_09_Projecoes.pdf".
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (translate)
    {
        printf("botao apertado\n");
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
        if (key == GLFW_KEY_Q)
        {
            camera_position_c += inc * camera_up_vector;
        }
        if (key == GLFW_KEY_E)
        {
            camera_position_c -= inc * camera_up_vector;
        }
    }

    if (lookAt)
    {
        camera_lookat_l = glm::normalize(obj_load->bbox_middle - camera_position_c);
    }
    camera_up_vector = -glm::normalize(glm::cross(camera_lookat_l, glm::cross(camera_lookat_l, camera_up_vector)));
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
        //float phimax = 3.141592f / 2;
        //float phimin = -phimax;

        if (lookAt)
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
        //float dx = xpos - g_LastCursorPosX;
        //float dy = ypos - g_LastCursorPosY;
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

std::tuple<float *, int> transformed(glm::mat4 P, glm::mat4 V, glm::mat4 M, MyObj *obj_load, float near, float far)
{
    float *vert = obj_load->Vert;
    float *ret = new float[obj_load->NumTris * 12];
    float near_t = near;
    float far_t = far;

    Matrices *mat454 = new Matrices();

    int numtris = obj_load->NumTris;

    glm::mat4 pvm = P * V * M;

    int i = 0;
    int indi = 0, indj = 0;
    for (int j = 0; j < obj_load->NumTris; j++)
    {
        indj = 9 * j;
        float *vert = obj_load->Vert;
        glm::vec4 v1 = glm::vec4(vert[indj + 0], vert[indj + 1], vert[indj + 2], 1.0f);
        glm::vec4 v2 = glm::vec4(vert[indj + 3], vert[indj + 4], vert[indj + 5], 1.0f);
        glm::vec4 v3 = glm::vec4(vert[indj + 6], vert[indj + 7], vert[indj + 8], 1.0f);

        glm::vec4 v1T = pvm * v1;
        glm::vec4 v2T = pvm * v2;
        glm::vec4 v3T = pvm * v3;

        if (v1T.w <= 0 || v2T.w <= 0 || v3T.w <= 0 || v1T.z <= near_t || v2T.z <= near_t || v3T.z <= near_t || v1T.z >= far_t || v2T.z >= far_t || v3T.z >= far_t)
        {
            numtris--;
            continue;
        }

        indi = 12 * i;
        ret[indi + 0] = v1T.x;
        ret[indi + 1] = v1T.y;
        ret[indi + 2] = v1T.z;
        ret[indi + 3] = v1T.w;

        ret[indi + 4] = v2T.x;
        ret[indi + 5] = v2T.y;
        ret[indi + 6] = v2T.z;
        ret[indi + 7] = v2T.w;

        ret[indi + 8] = v3T.x;
        ret[indi + 9] = v3T.y;
        ret[indi + 10] = v3T.z;
        ret[indi + 11] = v3T.w;

        i++;
    }
    float *ret2 = new float[numtris * 12];
    memcpy(ret2, ret, numtris * 12 * sizeof(float));
    free(ret);
    return std::make_tuple(ret2, numtris);
}

float *wdividi(float *PVMViC, int numtrisPVM)
{
    float *div = new float[numtrisPVM * 12];
    int ind = 0;
    for (int i = 0; i < numtrisPVM; i++)
    {
        ind = 12 * i;

        div[ind + 0] = PVMViC[ind + 0] / PVMViC[ind + 3];
        div[ind + 1] = PVMViC[ind + 1] / PVMViC[ind + 3];
        div[ind + 2] = PVMViC[ind + 2] / PVMViC[ind + 3];
        div[ind + 3] = PVMViC[ind + 3] / PVMViC[ind + 3];

        div[ind + 4] = PVMViC[ind + 4] / PVMViC[ind + 7];
        div[ind + 5] = PVMViC[ind + 5] / PVMViC[ind + 7];
        div[ind + 6] = PVMViC[ind + 6] / PVMViC[ind + 7];
        div[ind + 7] = PVMViC[ind + 7] / PVMViC[ind + 7];

        div[ind + 8] = PVMViC[ind + 8] / PVMViC[ind + 11];
        div[ind + 9] = PVMViC[ind + 9] / PVMViC[ind + 11];
        div[ind + 10] = PVMViC[ind + 10] / PVMViC[ind + 11];
        div[ind + 11] = PVMViC[ind + 11] / PVMViC[ind + 11];
    }
    memcpy(PVMViC, div, numtrisPVM * 12 * sizeof(float));
    free(div);
    return PVMViC;
}

std::tuple<float *, int> glCullFace(int mode, float *vert, int numtris)
{
    float a = 0.0f;
    int indj = 0;
    int indi = 0;
    float *ret = new float[numtris * 12];
    int goodTris = numtris;
    int i = 0;
    for (int j = 0; j < numtris; j++)
    {
        indj = 12 * j;
        a = vert[indj + 0] * vert[indj + 5] - vert[indj + 4] * vert[indj + 1] + vert[indj + 4] * vert[indj + 9] - vert[indj + 8] * vert[indj + 5] + vert[indj + 8] * vert[indj + 1] - vert[indj + 0] * vert[indj + 9];

        a = a / 2;

        if (mode == 1)
        {
            if (a < 0)
            {
                goodTris--;
                continue;
            }
            indi = 12 * i;
            ret[indi + 0] = vert[indj + 0];
            ret[indi + 1] = vert[indj + 1];
            ret[indi + 2] = vert[indj + 2];
            ret[indi + 3] = vert[indj + 3];

            ret[indi + 4] = vert[indj + 4];
            ret[indi + 5] = vert[indj + 5];
            ret[indi + 6] = vert[indj + 6];
            ret[indi + 7] = vert[indj + 7];

            ret[indi + 8] = vert[indj + 8];
            ret[indi + 9] = vert[indj + 9];
            ret[indi + 10] = vert[indj + 10];
            ret[indi + 11] = vert[indj + 11];
            i++;
        }
        else
        {
            if (a > 0)
            {
                goodTris--;
                continue;
            }
            indi = 12 * i;
            ret[indi + 0] = vert[indj + 0];
            ret[indi + 1] = vert[indj + 1];
            ret[indi + 2] = vert[indj + 2];
            ret[indi + 3] = vert[indj + 3];

            ret[indi + 4] = vert[indj + 4];
            ret[indi + 5] = vert[indj + 5];
            ret[indi + 6] = vert[indj + 6];
            ret[indi + 7] = vert[indj + 7];

            ret[indi + 8] = vert[indj + 8];
            ret[indi + 9] = vert[indj + 9];
            ret[indi + 10] = vert[indj + 10];
            ret[indi + 11] = vert[indj + 11];
            i++;
        }
    }

    std::memcpy(vert, ret, goodTris * 12 * sizeof(float));
    free(ret);
    return std::make_tuple(vert, goodTris);
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

    if (argc == 1)
    {
        printf("nenhum arquivo carregado.\n");
        return 0;
    }

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

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

    char *filename = argv[1];

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

    no_shade_index = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "none_v");
    no_shade_index_F = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "none");
    gourAD_shade_index = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "gourAD");
    gourADS_shade_index = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "gourADS");
    phong_shade_index = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "phong");

    GLuint VAO = BuildTriangles(obj_load);

    camera_position_c.x = obj_load->bbox_middle.x;
    camera_position_c.y = obj_load->bbox_middle.y;
    camera_position_c.z = 2 * std::max((obj_load->bbox_max.y - obj_load->bbox_min.y) * (1.0 / tan(45)), (obj_load->bbox_max.x - obj_load->bbox_min.x) * (1.0 / tan(45)));

    camera_position_O = camera_position_c;
    camera_lookat_l = glm::normalize(obj_load->bbox_middle - camera_position_c);
    direction = camera_lookat_l;
    camera_lookat_l_O = camera_lookat_l;
    camera_up_vector_O = camera_up_vector;

    camera_side_vector = glm::normalize(glm::cross(camera_lookat_l, camera_up_vector));

    float near = 0.1;
    float far = 3000;
    int render_mode = 0;
    int ccw = 1;
    int backFaceCull = 0;

    float vfov = 60;
    float hfov = vfov;
    int shad_tipe;
    int CloseGL = 1;
    Matrices *mat = new Matrices();
    View = glm::lookAt(camera_position_c, obj_load->bbox_middle, camera_up_vector);
    mat->inicialize(camera_position_c);
    lookAt = 0;
    int fps = 0;
    int showfps = 0;
    float time = glfwGetTime();
    GLuint VBO_close;
    glGenBuffers(1, &VBO_close);

    GLuint VAO_close;
    glGenVertexArrays(1, &VAO_close);
    glBindVertexArray(VAO_close);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_close);
    glBufferData(GL_ARRAY_BUFFER, obj_load->NumTris * 12 * sizeof(float), obj_load->Vert, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ShaderInfo shadersClose[] =
        {
            {GL_VERTEX_SHADER, "../../closeVert.glsl"},
            {GL_FRAGMENT_SHADER, "../../closeFrag.glsl"},
            {GL_NONE, NULL}};

    GLuint programCL = LoadShaders(shadersClose);

    model_color_CL = glGetUniformLocation(programCL, "model_color");

    mat->print_vector(mat->position);

    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(interface))
    {
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glfwMakeContextCurrent(window);
        static const float black[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, black);

        if (CloseGL == 0)
        {
            glUseProgram(program);
            switch (shading_index)
            {
            case 0:
                glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &no_shade_index);
                glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &no_shade_index_F);
                break;
            case 1:
                glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &gourAD_shade_index);
                glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &no_shade_index_F);
                break;
            case 2:
                glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &gourADS_shade_index);
                glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &no_shade_index_F);
                break;
            case 3:
                glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &no_shade_index);
                glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &phong_shade_index);
                break;
            }

            switch (render_mode)
            {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;

            case 1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            default:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            }
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
                    camera_lookat_l = glm::normalize(obj_load->bbox_middle - camera_position_c);
                    View = glm::lookAt(camera_position_c, obj_load->bbox_middle, camera_up_vector);
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
                    printf("%d\n", translate);
                    camera_lookat_l = direction;
                    View = glm::lookAt(camera_position_c, camera_lookat_l + camera_position_c, camera_up_vector);
                }
            }
            g_ScreenRatio = tan(glm::radians(hfov / 1)) / tan(glm::radians(vfov / 1));
            Projection = glm::perspective(glm::radians(vfov), g_ScreenRatio, near, far);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, NumVertices);
            glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(View));
            glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(Projection));
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glUniform3fv(model_color, 1, glm::value_ptr(Color_model));
        }
        else
        {
            glUseProgram(programCL);
            camera_lookat_l = direction;
            glDisable(GL_CULL_FACE);
            mat->position = camera_position_c;
            mat->n = camera_lookat_l;
            mat->v = camera_up_vector;
            mat->u = glm::normalize(glm::cross(camera_lookat_l, camera_up_vector));
            ViewGL = mat->view_matriz();
            ModelGL = mat->model();
            ProjectionGL = mat->proj_matriz(vfov, hfov, near, far);

            int numtrisPVM = 0;
            float *PVMViC;
            int flag = 1;

            switch (render_mode)
            {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;

            case 1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            default:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            }

            //float near_C = (near + fabs(mat->position.z)) * (mat->position.z/fabs(mat->position.z));
            //float far_C = (far + fabs(mat->position.z)) * (mat->position.z/fabs(mat->position.z));
            float near_C = near;
            float far_C = far;

            std::tie(PVMViC, numtrisPVM) = transformed(ProjectionGL, ViewGL, model, obj_load, near_C, far_C);

            float *wdiv = wdividi(PVMViC, numtrisPVM);
            float *culled;
            if (backFaceCull)
            {
                std::tie(culled, numtrisPVM) = glCullFace(ccw, wdiv, numtrisPVM);
            }
            else
            {
                culled = wdiv;
            }
            glBindVertexArray(VAO_close);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_close);
            glBufferSubData(GL_ARRAY_BUFFER, 0, numtrisPVM * 12 * sizeof(GLfloat), culled);
            glUniform3fv(model_color_CL, 1, glm::value_ptr(Color_model));
            glDrawArrays(GL_TRIANGLES, 0, numtrisPVM * 3);
            free(wdiv);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        fps++;

        glfwSwapInterval(0);

        if (glfwGetTime() - time >= 1)
        {
            showfps = fps / (glfwGetTime() - time);
            fps = 0;
            time = glfwGetTime();
        }

        glfwMakeContextCurrent(interface);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("My window"); // create window

        ImGui::Text("Application framerate (%.d FPS)", showfps);
        ImGui::RadioButton("OpenGL", &CloseGL, 0);
        ImGui::SameLine();
        ImGui::RadioButton("CloseGL", &CloseGL, 1);
        ImGui::NewLine();
        ImGui::RadioButton("Look At", &lookAt, 1);
        ImGui::SameLine();
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
        ImGui::SameLine();
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
        ImGui::Text("shading");
        ImGui::NewLine();
        ImGui::RadioButton("None", &shading_index, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Gouraud Shading AD", &shading_index, 1);
        ImGui::NewLine();
        ImGui::RadioButton("Gouraud Shading ADS", &shading_index, 2);
        ImGui::SameLine();
        ImGui::RadioButton("Phong Shading", &shading_index, 3);
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
        glfwSwapInterval(0);
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