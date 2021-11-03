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
    glm::vec4 eye_pos;

    void setDepth_Z(int w, int h)
    {
        this->depth_z = new float[w * h * 4];
    }
    void setColor(glm::vec3 color)
    {
        this->color = glm::vec4(color, 1.0);
    }
    void setEyePos(glm::vec4 eye)
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
    }

    std::tuple<float *, int> transformed(glm::mat4 P, glm::mat4 V, glm::mat4 M, MyObj *obj_load, float near, float far)
    {
        float *vert = obj_load->Vert;
        float *ret = new float[obj_load->NumTris * 12];
        float near_t = near;
        float far_t = far;

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
    void Proj_screen(glm::mat4 Proj, glm::mat4 VP, int mode)
    {
        int goodTris = this->tris;
        int i = 0;
        int indi = 0, indj = 0;
        for (int j = 0; j < this->tris; j++)
        {
            indj = 9 * j;
            glm::vec4 v1 = glm::vec4(eye_pos[indj + 0], eye_pos[indj + 1], eye_pos[indj + 2], eye_pos[indj + 3]);
            glm::vec4 v2 = glm::vec4(eye_pos[indj + 4], eye_pos[indj + 5], eye_pos[indj + 6], eye_pos[indj + 7]);
            glm::vec4 v3 = glm::vec4(eye_pos[indj + 8], eye_pos[indj + 9], eye_pos[indj + 10], eye_pos[indj + 11]);

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
        }
    }
};