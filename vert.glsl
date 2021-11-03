#version 400 core
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormalVertex;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 model_color;

out vec4 intercolor;
out vec4 position_world;
out vec4 normal;

subroutine void vertex_type(vec4 normal, vec4 position_world);

subroutine uniform vertex_type vertex_shading_type;


subroutine(vertex_type) void none_v(vec4 normal, vec4 position_world)
{
    intercolor = vec4(model_color.x,model_color.y,model_color.z,1);
}

subroutine(vertex_type) void gourAD(vec4 normal, vec4 position_world)
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

subroutine(vertex_type) void gourADS(vec4 normal, vec4 position_world)
{
    vec3 Kd = vec3(model_color.x,model_color.y,model_color.z);
    vec3 Ks = vec3(0.8,0.8,0.8);
    vec3 Ka = vec3(0.1,0.1,0.1) + vec3(model_color.x,model_color.y,model_color.z);
    float q = 16.0f;

    vec3 light_spectrum = vec3(1.0,1.0,1.0);
    vec3 ambient_light_spectrum = vec3(0.2,0.2,0.2);
    
    vec4 n = normalize(normal);

    vec4 camera_position = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 p = position_world;

    vec4 light_src_position = inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f);
    vec4 l = normalize(light_src_position - p);

    vec4 v = normalize(camera_position - p);

    vec4 r = normalize(-l + 2 * n * dot(n, l));
    float diffuse = max( 0 , dot(n, l));
    float specular  = pow( max( 0, dot(r, v)), q);

    vec3 aux = (Ka * ambient_light_spectrum) + (Kd * light_spectrum * diffuse) + (Ks * light_spectrum * specular);

    intercolor = vec4(aux.x, aux.y, aux.z, 1.0f);
}

void
main()
{
    position_world = model * vPosition;
    gl_Position = projection * view * position_world;

    normal = inverse(transpose(model))* vNormalVertex;
    
    normal.w = 0.0;

    vertex_shading_type(normal, position_world);

}
