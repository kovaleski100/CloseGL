#version 450 core
uniform mat4 view;


in vec4 intercolor;
in vec4 position_world;
in vec4 normal;
uniform int phong;
uniform vec3 model_color;
uniform int shading_index;

out vec4 fColor;

void main()
{
    if(phong==1 && shading_index > 2)
    {
        vec3 Kd = vec3(model_color.x,model_color.y,model_color.z);
        vec3 Ks = vec3(model_color.x,model_color.y,model_color.z);
        vec3 Ka = vec3(model_color.x,model_color.y,model_color.z);
        float q = 5.0f;


        vec4 camera_position = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vec4 p = position_world;
        vec4 n = normalize(normal);

        vec4 light_src_position = inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f);
        vec4 l = normalize(light_src_position - p);

        vec4 v = normalize(camera_position - p);

        vec4 r = -l + 2 * n * dot(n, l);
        float diffuse = max( 0 , dot(n, l));
        float specular  = pow( max( 0, dot(r, v)), q);



        vec3 light_spectrum = vec3(1.0,1.0,1.0);
        vec3 ambient_light_spectrum = vec3(0.2,0.2,0.2);

        vec3 aux = Ka * ambient_light_spectrum + Kd * light_spectrum * diffuse + Ks * light_spectrum * specular;

        fColor = vec4(aux.x ,aux.y ,aux.z , 1.0f);
    }
    else
    {
        fColor = intercolor;
    }
}
