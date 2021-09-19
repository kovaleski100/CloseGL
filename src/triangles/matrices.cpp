#include <glm/vec3.hpp> // glm::vec3 
#include <glm/vec4.hpp> // glm::vec4 
#include <glm/mat4x4.hpp> // glm::mat4
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective 


class Matrices
{
public:
    glm::vec3 position;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 n;
    float hfov = 60.0f;
    float vfov = 60.0f;

    void inicialize()
    {
        position = glm::vec3(0,0,0);
        u = glm::vec3(1,0,0);
        v = glm::vec3(0,1,0);
        n = glm::vec3(0,0,-1);
        vfov = 60;
        hfov = 60;
    }

    void camera_position(glm::vec3 c_position)
    {
        position = c_position;
    }
    void change_u(glm::vec3 u_new)
    {
        u = u_new;
    }
    void change_u(glm::vec3 v_new)
    {
        v = v_new;
    }
    void change_u(glm::vec3 z_new)
    {
        n = z_new;
    }

    glm::mat4 view_matriz(){
        glm::mat4 V = glm::mat4(
            u.x, u.y, u.z, glm::dot(-position,u),
            v.x, v.y, v.z, glm::dot(-position,v),
            n.x, n.y, n.z, glm::dot(-position,n),
            0.0f,0.0f,0.0f,1.0f
        );
        return V;
    }

    glm::mat4 proj_matriz(float near, float far){

        float t = fabs(near) * tanf(glm::radians(vfov) / 2.0f);
        float b = -t;
        float r = t * tan(glm::radians(hfov / 1)) / tan(glm::radians(vfov / 1));;
        float l = -r;


        glm::mat4 P = glm::mat4(
            2*near/(r-l), 0.0f     , (r+l)/(r-l)           , 0.0f,
            0.0f     , 2*near/(t-b), (t+b)/(t-b)           , 0.0f,
            0.0f     , 0.0f     , -(far+near)/(far-near), -2*far*near/(far-near),
            0.0f     , 0.0f     , -1.0f                 , 0.0f
        );  
        return P;
    }


    glm::mat4 Viewport_matriz(){
        glm::mat4 view = glm::mat4(
            u.x, u.y, u.z, glm::dot(-position,u),
            v.x, v.y, v.z, glm::dot(-position,v),
            n.x, n.y, n.z, glm::dot(-position,n),
            0.0f,0.0f,0.0f,1.0f
        );
        return view;
    }
};