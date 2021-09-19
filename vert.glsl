#version 400 core
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormalVertex;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 model_color;

uniform int shading_index;

out mat4 view_f;
out vec4 intercolor;
out vec4 position_world;
out vec4 normal;

uniform int phong;


/*
subroutine void vertex_type();

subroutine(vertex_type) void none()
{
    intercolor = vec4(model_color.x,model_color.y,model_color.z,1);
}

subroutine(vertex_type) void gourAD()
{
    vec3 Kd = vec3(model_color.x,model_color.y,model_color.z);;
    vec3 Ks = vec3(model_color.x,model_color.y,model_color.z);;
    vec3 Ka = vec3(model_color.x,model_color.y,model_color.z);;
    vec3 light_spectrum = vec3(1.0, 1.0, 1.0);
    vec3 ambient_light_spectrum = vec3(0.2, 0.2, 0.2);

    vec4 camera_position = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 p = position_world;

    vec4 light_src_position = inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f);
    vec4 l = normalize(light_src_position - p);


    float diffuse = max( 0 , dot(normal, l));

    vec3 aux = (Ka * ambient_light_spectrum + Kd * light_spectrum * diffuse);

    intercolor = vec4(aux.x,aux.y,aux.z,1f);
}

subroutine(vertex_type) void gourADS()
{
    vec3 Kd = vec3(model_color.x,model_color.y,model_color.z);;
    vec3 Ks = vec3(model_color.x,model_color.y,model_color.z);;
    vec3 Ka = vec3(model_color.x,model_color.y,model_color.z);;
    float q = 5.0f;

    vec3 light_spectrum = vec3(1.0,1.0,1.0);
    vec3 ambient_light_spectrum = vec3(0.2,0.2,0.2);

    vec4 camera_position = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 p = position_world;

   vec4 light_src_position = inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f);
    vec4 l = normalize(light_src_position - p);

    vec4 v = normalize(camera_position - p);

    vec4 r = -l + 2 * normal * dot(normal, l);
    float diffuse = max( 0 , dot(normal, l));
    float specular  = pow( max( 0, dot(r, v)), q);

    vec3 aux = ( Ka * ambient_light_spectrum + Kd * light_spectrum * diffuse + Ks * light_spectrum * specular);

    intercolor = vec4(aux.x, aux.y, aux.z, 1.0f);
}

subroutine(vertex_type) void phong()
{
    //position_world = model * model_coefficients;

    //normal = inverse(transpose(view*model)) * normal_coefficients;
    //normal = normal_coefficients;
    //normal.w = 0.0;
    int opa=0;

    //out_color = vec3(0.5f, 0.0f, 0.5f);
}

subroutine uniform vertex_type vertex_shading_type;

*/
void
main()
{
    position_world = model * vPosition;
    gl_Position = projection * view * position_world;
    view_f = view;

    normal = inverse(transpose(model))* vNormalVertex;

    if(shading_index == 0)
    {
         intercolor = vec4(model_color.x,model_color.y,model_color.z,1);
    }
    else if(shading_index == 1)
    {
        vec3 Kd = vec3(model_color.x,model_color.y,model_color.z);
        vec3 Ks = vec3(model_color.x,model_color.y,model_color.z);
        vec3 Ka = vec3(model_color.x,model_color.y,model_color.z);
        vec3 light_spectrum = vec3(1.0, 1.0, 1.0);
        vec3 ambient_light_spectrum = vec3(0.2, 0.2, 0.2);

        vec4 camera_position = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vec4 p = position_world;

        vec4 light_src_position = inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f);
        vec4 l = normalize(light_src_position - p);


        float diffuse = max( 0 , dot(normal, l));

        vec3 aux = (Ka * ambient_light_spectrum + Kd * light_spectrum * diffuse);

        intercolor = vec4(aux.x, aux.y, aux.z, 1.0f);
    }
    else if(shading_index == 2)
    {
        vec3 Kd = vec3(model_color.x,model_color.y,model_color.z);
        vec3 Ks = vec3(model_color.x,model_color.y,model_color.z);
        vec3 Ka = vec3(model_color.x,model_color.y,model_color.z);
        float q = 5.0f;

        vec3 light_spectrum = vec3(1.0,1.0,1.0);
        vec3 ambient_light_spectrum = vec3(0.2,0.2,0.2);

        vec4 camera_position = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vec4 p = position_world;

        vec4 light_src_position = inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f);
        vec4 l = normalize(light_src_position - p);

        vec4 v = normalize(camera_position - p);

        vec4 r = -l + 2 * normal * dot(normal, l);
        float diffuse = max( 0 , dot(normal, l));
        float specular  = pow( max( 0, dot(r, v)), q);

        vec3 aux = ( Ka * ambient_light_spectrum + Kd * light_spectrum * diffuse + Ks * light_spectrum * specular);

        intercolor = vec4(aux.x, aux.y, aux.z, 1.0f);
    }
    else
    {
        //phong = 1;
        //intercolor = vec4(model_color.x,model_color.y,model_color.z,1);
        
    }

}
