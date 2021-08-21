
#include <bits/stdc++.h>
#include <glm/vec3.hpp>

#define MAX_MATERIAL_COUNT 1

class MyObj
{
public:
    std::string Obj_name;
    int NumTris;
    float* Vert;
    float* Vert_Normal;
    struct Triangles
    {
        /* data */
        glm::vec3 v0;
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 Norm[3];
        glm::vec3 face_normal;
        unsigned char Color[3];
    };

    glm::vec3 bbox_min = glm::vec3();
	glm::vec3 bbox_max = glm::vec3();
	glm::vec3 bbox_middle = glm::vec3(0.0f);

    float max(float x1, float x2)
    {
        if(x1<x2)
        {
            return x2;
        }
        else
        {
            return x1;
        }
    }

    float min_v(float x1, float x2)
    {
        if(x1>x2)
        {
            return x2;
        }
        else
        {
            return x1;
        }
    }

    void find_max(Triangles* Tris)
    {
        int i;
        bbox_max = Tris[0].v0;

        for (i = 0; i < NumTris; i++)
        {
            bbox_max.x = max(bbox_max.x,max(Tris[i].v0.x,max(Tris[i].v1.x,Tris[i].v2.x)));
            bbox_max.y = max(bbox_max.y,max(Tris[i].v0.y,max(Tris[i].v1.y,Tris[i].v2.y)));
            bbox_max.z = max(bbox_max.z,max(Tris[i].v0.z,max(Tris[i].v1.z,Tris[i].v2.z)));
        }
        //printf("max = %f %f %f",bbox_max.x, bbox_max.y, bbox_max.z);
    }

    void find_min(Triangles* Tris)
    {   
        int i;
        bbox_min = Tris[0].v0;
        for (i = 0; i < NumTris; i++)
        {
            //    vertex coordinates
            bbox_min.x = min_v(bbox_min.x,min_v(Tris[i].v0.x,min_v(Tris[i].v1.x,Tris[i].v2.x)));
            bbox_min.y = min_v(bbox_min.y,min_v(Tris[i].v0.y,min_v(Tris[i].v1.y,Tris[i].v2.y)));
            bbox_min.z = min_v(bbox_min.z,min_v(Tris[i].v0.z,min_v(Tris[i].v1.z,Tris[i].v2.z)));
        }
        //printf("min = %f %f %f",bbox_min.x, bbox_min.y, bbox_min.z);
    }

    void load_obj(char *FileName)
    {
        glm::vec3 ambient[MAX_MATERIAL_COUNT];
        glm::vec3 diffuse[MAX_MATERIAL_COUNT];
        glm::vec3 specular[MAX_MATERIAL_COUNT];
        float shine[MAX_MATERIAL_COUNT];

        int material_count;
        int color_index[3];
        int i;
        char ch;

        FILE *fp = fopen(FileName, "r");
        if (fp == NULL)
        {
            printf("ERROR: unable to open TriObj [%s]!\n", FileName);
            exit(1);
        }

        fscanf(fp, "%c", &ch);
        while (ch != '\n') // skip the first line – object’s name
            fscanf(fp, "%c", &ch);
        //
        fscanf(fp, "# triangles = %d\n", &NumTris);           // read # of triangles
        fscanf(fp, "Material count = %d\n", &material_count); // read material count
                                                              //
        for (i = 0; i < material_count; i++)
        {
            fscanf(fp, "ambient color %f %f %f\n", &(ambient[i].x), &(ambient[i].y), &(ambient[i].z));
            fscanf(fp, "diffuse color %f %f %f\n", &(diffuse[i].x), &(diffuse[i].y), &(diffuse[i].z));
            fscanf(fp, "specular color %f %f %f\n", &(specular[i].x), &(specular[i].y), &(specular[i].z));
            fscanf(fp, "material shine %f\n", &(shine[i]));
        }
        //
        fscanf(fp, "%c", &ch);
        while (ch != '\n') // skip documentation line
            fscanf(fp, "%c", &ch);
        //
        //  allocate triangles for tri model
        //
        printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
        Triangles* Tris = new Triangles[NumTris];
        //
        for (i = 0; i < NumTris; i++) // read triangles
        {
            fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
                   &(Tris[i].v0.x), &(Tris[i].v0.y), &(Tris[i].v0.z),
                   &(Tris[i].Norm[0].x), &(Tris[i].Norm[0].y), &(Tris[i].Norm[0].z),
                   &(color_index[0]));
            fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
                   &(Tris[i].v1.x), &(Tris[i].v1.y), &(Tris[i].v1.z),
                   &(Tris[i].Norm[1].x), &(Tris[i].Norm[1].y), &(Tris[i].Norm[1].z),
                   &(color_index[1]));
            fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
                   &(Tris[i].v2.x), &(Tris[i].v2.y), &(Tris[i].v2.z),
                   &(Tris[i].Norm[2].x), &(Tris[i].Norm[2].y), &(Tris[i].Norm[2].z),
                   &(color_index[2]));
            fscanf(fp, "face normal %f %f %f\n", &(Tris[i].face_normal.x), &(Tris[i].face_normal.y),
                   &(Tris[i].face_normal.z));
            //
            Tris[i].Color[0] = (unsigned char)(int)(255 * (diffuse[color_index[0]].x));
            Tris[i].Color[1] = (unsigned char)(int)(255 * (diffuse[color_index[0]].y));
            Tris[i].Color[2] = (unsigned char)(int)(255 * (diffuse[color_index[0]].z));
        }
        fclose(fp);

        //
        //     For use in the vertex buffer objects in your application, pack the vertex and normal data
        //           into vectors
        //
        Vert = new float[9 * NumTris];
        Vert_Normal = new float[9 * NumTris];

        find_min(Tris);
        find_max(Tris);


        for (i = 0; i < NumTris; i++)
        {
            //    vertex coordinates
            Vert[9 * i + 0] = Tris[i].v0.x;
            Vert[9 * i + 1] = Tris[i].v0.y;
            Vert[9 * i + 2] = Tris[i].v0.z;
            Vert[9 * i + 3] = Tris[i].v1.x;
            Vert[9 * i + 4] = Tris[i].v1.y;
            Vert[9 * i + 5] = Tris[i].v1.z;
            Vert[9 * i + 6] = Tris[i].v2.x;
            Vert[9 * i + 7] = Tris[i].v2.y;
            Vert[9 * i + 8] = Tris[i].v2.z;

            //    vertex normal coordinates
            Vert_Normal[9 * i] = Tris[i].Norm[0].x;
            Vert_Normal[9 * i + 1] = Tris[i].Norm[0].y;
            Vert_Normal[9 * i + 2] = Tris[i].Norm[0].z;
            Vert_Normal[9 * i + 3] = Tris[i].Norm[1].x;
            Vert_Normal[9 * i + 4] = Tris[i].Norm[1].y;
            Vert_Normal[9 * i + 5] = Tris[i].Norm[1].z;
            Vert_Normal[9 * i + 6] = Tris[i].Norm[2].x;
            Vert_Normal[9 * i + 7] = Tris[i].Norm[2].y;
            Vert_Normal[9 * i + 8] = Tris[i].Norm[2].z;

            glm::vec3 bbox_diam = glm::vec3(bbox_max + bbox_min);
		    bbox_middle = glm::vec3(bbox_diam.x/2, bbox_diam.y/2, bbox_diam.z/2);
        }
    }
};