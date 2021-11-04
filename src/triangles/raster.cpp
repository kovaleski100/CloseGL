#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <bits/stdc++.h>
#include <string>
#include <X11/Xlib.h>
#include <math.h>

#include "Myobj.cpp"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


#define N_INTERPIE 16

#define XCREEN 0
#define YCREEN 1
#define WCREEN 2
#define XNORMAL 3
#define YNORMAL 4
#define ZNORMAL 5
#define XEYE 6
#define YEYE 7
#define ZEYE 8
#define ZNDC 9
#define RCOL 10
#define GCOL 11
#define BCOL 12
#define STEXT 13
#define TTEXT 14
#define ONE 15


class Raster
{
public:
    glm::vec4 color;
    float *depth_z;
    float *world;
    float *NDC;
    float *normals;
    float *screen;
    int tris;
    float near;
    float far;
    float *eye_pos;

    void setDepth_Z(int w, int h)
    {
        this->depth_z = new float[w * h * 4];
    }
    void setColor(glm::vec3 color)
    {
        this->color = glm::vec4(color, 1.0);
    }
    void setEyePos(float *eye)
    {
        this->eye_pos = eye;
    }
    void setFov(float near, float far)
    {
        this->near = near;
        this->far = far;
    }
    Raster(int tris, float *vert, float *norm)
    {
        this->tris = tris;
        this->world = new float[tris*12];
        this->normals = new float[tris*12];

        int indi1 = 0;
        int indi2 = 0;
        for(int i= 0; i<tris; i++)
        {
            indi1 = 12*i;
            indi2 = 9*i;
            this->world[indi1+1] = vert[indi2+1];
            this->world[indi1+0] = vert[indi2+0];
            this->world[indi1+2] = vert[indi2+2];
            this->world[indi1+3] = 1.0f;
            this->world[indi1+4] = vert[indi2+3];
            this->world[indi1+5] = vert[indi2+4];
            this->world[indi1+6] = vert[indi2+5];
            this->world[indi1+7] = 1.0f;
            this->world[indi1+8] = vert[indi2+6];
            this->world[indi1+9] = vert[indi2+7];
            this->world[indi1+10] = vert[indi2+8];
            this->world[indi1+11] = 1.0f;
        
            this->normals[indi1+1] = norm[indi2+1];
            this->normals[indi1+0] = norm[indi2+0];
            this->normals[indi1+2] = norm[indi2+2];
            this->normals[indi1+3] = 1.0f;
            this->normals[indi1+4] = norm[indi2+3];
            this->normals[indi1+5] = norm[indi2+4];
            this->normals[indi1+6] = norm[indi2+5];
            this->normals[indi1+7] = 1.0f;
            this->normals[indi1+8] = norm[indi2+6];
            this->normals[indi1+9] = norm[indi2+7];
            this->normals[indi1+10] = norm[indi2+8];
            this->normals[indi1+11] = 1.0f;
        }
    }

    void destructor()
    {
        free(world);
        free(screen);
        free(NDC);
        free(normals);
        free(eye_pos);
    }

    void Proj_screen(glm::mat4 Proj, glm::mat4 VP, int mode, glm::mat4 VPIT)
    {
        float* normal_remains = new float[this->tris*12]; //cada tri tem 3 vertices em coord homo, numero máximo de resposta
        float* world_remains = new float[this->tris*12];
        float* eye_remains = new float[this->tris*12];

        int goodTris = this->tris;
        int i = 0;
        int indi = 0, indj = 0;
        for (int j = 0; j < this->tris; j++)
        {
            indj = 12 * j;
            glm::vec4 v1 = glm::vec4(this->eye_pos[indj + 0], this->eye_pos[indj + 1], this->eye_pos[indj + 2], this->eye_pos[indj + 3]);
            glm::vec4 v2 = glm::vec4(this->eye_pos[indj + 4], this->eye_pos[indj + 5], this->eye_pos[indj + 6], this->eye_pos[indj + 7]);
            glm::vec4 v3 = glm::vec4(this->eye_pos[indj + 8], this->eye_pos[indj + 9], this->eye_pos[indj + 10], this->eye_pos[indj + 11]);

            glm::vec4 v1T = Proj * v1;
            glm::vec4 v2T = Proj * v2;
            glm::vec4 v3T = Proj * v3;

            if (v1T.w <= 0 || v2T.w <= 0 || v3T.w <= 0 
            || v1T.z <= this->near || v2T.z <= this->near || v3T.z <= this->near 
            || v1T.z >= this->far || v2T.z >= this->far || v3T.z >= this->far
            || abs(v1T.x) >= abs(v1T.w) || abs(v2T.x) >= abs(v2T.w) || abs(v3T.x) >= abs(v3T.w))
            {
                goodTris--;
                continue;
            }

            glm::vec4 auxv1T = v1T/ v1T.w;
            glm::vec4 auxv2T = v2T/ v2T.w;
            glm::vec4 auxv3T = v3T/ v3T.w;

            float a = auxv1T.x * auxv2T.y - auxv2T.x * auxv1T.y 
                    + auxv2T.x * auxv3T.y - auxv3T.x * auxv2T.y 
                    + auxv3T.x * auxv1T.y - auxv1T.x * auxv3T.y;

            a = a / 2;

            if ((mode == 1 && a<0) || (mode = 0 && a >0))
            {
                    goodTris--;
                    continue;
            }

            glm::vec4 n1 = VPIT * glm::vec4(this->normals[indj + 0], this->normals[indj + 1], this->normals[indj + 2], this->normals[indj + 3]);
            glm::vec4 n2 = VPIT * glm::vec4(this->normals[indj + 4], this->normals[indj + 5], this->normals[indj + 6], this->normals[indj + 7]);
            glm::vec4 n3 = VPIT * glm::vec4(this->normals[indj + 8], this->normals[indj + 9], this->normals[indj + 10], this->normals[indj + 11]);

            indi = 12*i;
            normal_remains[indi + 0] = n1.x;
            normal_remains[indi + 1] = n1.y;
            normal_remains[indi + 2] = n1.z;
            normal_remains[indi + 3] = n1.w;
            normal_remains[indi + 4] = n2.x;
            normal_remains[indi + 5] = n2.y;
            normal_remains[indi + 6] = n2.z;
            normal_remains[indi + 7] = n2.w;
            normal_remains[indi + 8] = n3.x;
            normal_remains[indi + 9] = n3.y;
            normal_remains[indi + 10] = n3.z;
            normal_remains[indi + 11] = n3.w;
            //guardamos as coord de mundo dos triangulos sobreviventes
            world_remains[indi + 0] = this->world[indj+0];
            world_remains[indi + 1] = this->world[indj+1];
            world_remains[indi + 2] = this->world[indj+2];
            world_remains[indi + 3] = this->world[indj+3];
            world_remains[indi + 4] = this->world[indj+4];
            world_remains[indi + 5] = this->world[indj+5];
            world_remains[indi + 6] = this->world[indj+6];
            world_remains[indi + 7] = this->world[indj+7];
            world_remains[indi + 8] = this->world[indj+8];
            world_remains[indi + 9] = this->world[indj+9];
            world_remains[indi + 10] = this->world[indj+10];
            world_remains[indi + 11] = this->world[indj+11];
            //guardamos os eye coord sobreviventes tbm
            eye_remains[indi + 0] = this->eye_pos[indj+0];
            eye_remains[indi + 1] = this->eye_pos[indj+1];
            eye_remains[indi + 2] = this->eye_pos[indj+2];
            eye_remains[indi + 3] = this->eye_pos[indj+3];
            eye_remains[indi + 4] = this->eye_pos[indj+4];
            eye_remains[indi + 5] = this->eye_pos[indj+5];
            eye_remains[indi + 6] = this->eye_pos[indj+6];
            eye_remains[indi + 7] = this->eye_pos[indj+7];
            eye_remains[indi + 8] = this->eye_pos[indj+8];
            eye_remains[indi + 9] = this->eye_pos[indj+9];
            eye_remains[indi + 10] = this->eye_pos[indj+10];
            eye_remains[indi + 11] = this->eye_pos[indj+11];
            
            glm::vec4 v1s = VP * v1T;
            glm::vec4 v2s = VP * v2T;
            glm::vec4 v3s = VP * v3T;
            
            this->screen[indi + 0] = v1s.x/v1s.w; //divide x y e z por w e mantem o w ´paara uso na interpolação
            this->screen[indi + 1] = v1s.y/v1s.w; //divisão perspectiva
            this->screen[indi + 2] = v1s.z/v1s.w;
            this->screen[indi + 3] = v1s.w;
            this->screen[indi + 4] = v2s.x/v2s.w;
            this->screen[indi + 5] = v2s.y/v2s.w;
            this->screen[indi + 6] = v2s.z/v2s.w;
            this->screen[indi + 7] = v2s.w;
            this->screen[indi + 8] = v3s.x/v3s.w;
            this->screen[indi + 9] = v3s.y/v3s.w;
            this->screen[indi + 10] = v3s.z/v3s.w;
            this->screen[indi + 11] = v3s.w;

            indi = 3*j;
            this->NDC[indi+0] = v1T.z/v1T.w;
            this->NDC[indi+1] = v2T.z/v2T.w;
            this->NDC[indi+2] = v3T.z/v3T.w;

            j++;            
        }
        free(this->normals);
        free(this->world);
        free(this->eye_pos);

        this->tris = goodTris;

        this->normals = new float[goodTris];
        this->world = new float[goodTris];
        this->eye_pos = new float[goodTris];

        std::memcpy(this->normals, normal_remains, this->tris*12*sizeof(float));
        std::memcpy(this->eye_pos, eye_remains, this->tris*12*sizeof(float));
        std::memcpy(this->world, world_remains, this->tris*12*sizeof(float));

        free(world_remains);
        free(eye_remains);
        free(normal_remains);
    }

    void getEye(glm::mat4 MV)
    {
        this->eye_pos = new float[this->tris*12];

        int indi = 0;
        for(int i =0; i< this->tris; i++)
        {
            indi = 12*i;
            glm::vec4 v1 = glm::vec4(this->world[indi + 0], this->world[indi + 1], this->world[indi + 2], this->world[indi + 3]);
            glm::vec4 v2 = glm::vec4(this->world[indi + 4], this->world[indi + 5], this->world[indi + 6], this->world[indi + 7]);
            glm::vec4 v3 = glm::vec4(this->world[indi + 8], this->world[indi + 9], this->world[indi + 10], this->world[indi + 11]);

            glm::vec4 v1l = MV * v1;
            glm::vec4 v2l = MV * v2;
            glm::vec4 v3l = MV * v3;

            this->eye_pos[indi + 0] = v1l.x;
            this->eye_pos[indi + 1] = v1l.y;
            this->eye_pos[indi + 2] = v1l.z;
            this->eye_pos[indi + 3] = v1l.w;
            this->eye_pos[indi + 4] = v2l.x;
            this->eye_pos[indi + 5] = v2l.y;
            this->eye_pos[indi + 6] = v2l.z;
            this->eye_pos[indi + 7] = v2l.w;
            this->eye_pos[indi + 8] = v3l.x;
            this->eye_pos[indi + 9] = v3l.y;
            this->eye_pos[indi + 10] = v3l.z;
            this->eye_pos[indi + 11] = v3l.w;
        }
    }
};