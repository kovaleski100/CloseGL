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

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <bits/stdc++.h>
#include <string>

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
    ArrayBuffer,
    NumBuffers
};
enum Attrib_IDs
{
    vPosition = 0
};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLuint NumVertices = 3;

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

GLuint BuildTriangles()
{

    GLfloat NDC_coefficients[] = {
        //    X      Y     Z     W
        -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f};
    GLuint VBO_NDC_coefficients_id;
    glGenBuffers(1, &VBO_NDC_coefficients_id);
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_NDC_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NDC_coefficients), NDC_coefficients);
    GLuint location = 0;            // "(location = 0)" em "shader_vertex.glsl"
    GLint number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLfloat color_coefficients[] = {
        //  R     G     B     A
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1;             // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLubyte indices[] = {0, 1, 2}; // GLubyte: valores entre 0 e 255 (8 bits sem sinal).
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos memória para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
    // caso contrário o VAO irá perder a informação sobre os índices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
    // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
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

    if (window == NULL)
        return 1;

    GLFWwindow *interface = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);

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


    ImVec4 clear_color = ImVec4(0.6f, 0.3f, 0.6f, 1.00f);

    glfwMakeContextCurrent(window);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);

    GLuint vertex_array_object_id = BuildTriangles();

    ShaderInfo shaders[] =
        {
            {GL_VERTEX_SHADER, "../../triangles.vert"},
            {GL_FRAGMENT_SHADER, "../../triangles.frag"},
            {GL_NONE, NULL}};

    GLuint program = LoadShaders(shaders);

    glVertexAttribPointer(vPosition, 2, GL_FLOAT,
                          GL_FALSE, 0, BUFFER_OFFSET(0));
    //glEnableVertexAttribArray(vPosition);

    
    std::cout<< glsl_version;
    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(interface))
    {
        static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};

        glClearBufferfv(GL_COLOR, 0, black);

        glUseProgram(program);
        glfwMakeContextCurrent(window);
        glBindVertexArray(vertex_array_object_id);
        //glBindVertexArray(VAOs[Triangles]);
        //glDrawArrays(GL_TRIANGLES, 0, NumVertices);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0);

        

        glfwSwapBuffers(window);
        glfwPollEvents();



        glfwMakeContextCurrent(interface);

        //static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};

        glClearBufferfv(GL_COLOR, 0, black);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("My window"); // create window
        if (ImGui::Button("Load"))
        {
            // call your loading code
        }
        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            // call your saving code
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(interface);
        glfwMakeContextCurrent(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
}
