#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4
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

    void inicialize(glm::vec3 newposition)
    {
        position = newposition;
        u = glm::vec3(1, 0, 0);
        v = glm::vec3(0, 1, 0);
        n = glm::vec3(0, 0, -1);
    }

    void camera_position(glm::vec3 c_position)
    {
        position = c_position;
    }
    void change_u(glm::vec3 u_new)
    {
        u = u_new;
    }
    void change_v(glm::vec3 v_new)
    {
        v = v_new;
    }
    void change_n(glm::vec3 z_new)
    {
        n = z_new;
    }

    float dotProd(glm::vec3 v1, glm::vec3 v2)
    {
        return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    }

    glm::mat4 view_matriz()
    {
        n = -n;
        glm::mat4 V = glm::mat4(
            u.x, u.y, u.z, dotProd(u, -position),
            v.x, v.y, v.z, dotProd(v, -position),
            n.x, n.y, n.z, dotProd(n, -position),
            0.0f, 0.0f, 0.0f, 1.0f);
        return glm::transpose(V);
    }

    glm::mat4 Viewport_matriz(int x, int y, int width, int height)
    {
        glm::mat4 viewport = glm::mat4(
            width / 2.0f, 0.0f, 0.0f, x + (width / 2.0f),
            0.0f, -height / 2.0f, 0.0f, y + (height / 2.0f),
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
        return viewport;
    }

    glm::mat4 proj_matriz(float vfov, float hfov, float near, float far)
    {

        float t = fabs(near) * tanf(glm::radians(vfov) / 2.0f);
        float b = -t;
        float r = t * tan(glm::radians(hfov / 1)) / tan(glm::radians(vfov / 1));
        ;
        float l = -r;

        glm::mat4 P = glm::mat4(
            (2 * near) / (r - l), 0.0f, (r + l) / (r - l), 0.0f,
            0.0f, (2 * near) / (t - b), (t + b) / (t - b), 0.0f,
            0.0f, 0.0f, -(far + near) / (far - near), -(2 * far * near) / (far - near),
            0.0f, 0.0f, -1.0f, 0.0f);
        return glm::transpose(P);
    }

    glm::mat4 model()
    {
        glm::mat4 M = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return M;
    }

    void print_vector(glm::vec4 vec)
    {
        std::cout << " x = " << vec.x << " y = " << vec.y << " z = " << vec.z << " w = " << vec.w;
    }

    void print_vector(glm::vec3 vec)
    {
        std::cout << " x = " << vec.x << " y = " << vec.y << " z = " << vec.z << " w = " << std::endl;
    }

    void print_matrix(glm::mat4 mat)
    {
        for (int i = 0; i < 4; i++)
        {
            print_vector(mat[i]);
            std::cout << "\n";
        }
    }

    glm::mat4 translate_matrix(float tx, float ty, float tz)
    {
        glm::mat4 M = glm::mat4(
            1.0f, 0.0f, 0.0f, tx,
            0.0f, 1.0f, 0.0f, ty,
            0.0f, 0.0f, 1.0f, tz,
            0.0f, 0.0f, 0.0f, 1.0f);
        return M;
    }

    float norm(glm::vec4 v)
    {
        float vx = v.x;
        float vy = v.y;
        float vz = v.z;

        return sqrt(vx * vx + vy * vy + vz * vz);
    }

    glm::mat4 rotate_matrix(float angle, glm::vec4 axis)
    {
        float coss = cos(angle);
        float seno = sin(angle);

        glm::vec4 v = axis / norm(axis);

        float vx = v.x;
        float vy = v.y;
        float vz = v.z;

        glm::mat4 Rotate = glm::mat4(
            vx*vx*(1.0f-coss)+coss    , vx*vy*(1.0f-coss)-vz*seno , vx*vz*(1-coss)+vy*seno , 0.0f ,
            vx*vy*(1.0f-coss)+vz*seno , vy*vy*(1.0f-coss)+coss    , vy*vz*(1-coss)-vx*seno , 0.0f ,
            vx*vz*(1-coss)-vy*seno    , vy*vz*(1-coss)+vx*seno    , vz*vz*(1.0f-coss)+coss , 0.0f ,
            0.0f                      , 0.0f                      , 0.0f                   , 1.0f
        );

        return glm::transpose(Rotate);
    }
};