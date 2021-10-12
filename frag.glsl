#version 450 core
uniform mat4 view;


in vec4 intercolor;
in vec4 position_world;
in vec4 normal;
uniform vec3 model_color;
uniform int shading_index;

out vec4 fColor;

subroutine void frag_type();

subroutine uniform frag_type frag_shading_type;

subroutine(frag_type) void none()
{
    fColor = intercolor;
}
subroutine(frag_type) void phong()
{

    // Obtemos a posição da câmera utilizando a inversa da matriz que define o sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(inverse(view) * vec4(2.0f, 2.0f, 2.0f, 1.0f));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = normalize(-l + 2*n * dot(n, l));

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd = vec3(intercolor.x, intercolor.y,intercolor.z);                      // Refletância difusa
    vec3 Ks = vec3(0.8,0.8,0.8);                // Refletância especular
    vec3 Ka = vec3(0.1,0.1,0.1) + vec3(intercolor.x, intercolor.y,intercolor.z);  // Refletância ambiente
    float q = 16.0;                             // Expoente especular para o modelo de iluminação de Phong

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term  = Ks * I * max(0, pow(dot(r, v), q));

    // Cor final do fragmento calculada com uma combinação dos termos e correção gamma, considerando monitor sRGB.
    vec3 aux = pow(lambert_diffuse_term + ambient_term + phong_specular_term, vec3(1.0,1.0,1.0)/2.2);
    fColor = vec4(aux, 1.0);
}




void main()
{
   frag_shading_type();
}
