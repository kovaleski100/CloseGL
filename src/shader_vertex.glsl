#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define PLANEE  6
#define CUBO 4
#define BARCOE 5
#define seenselect 7
#define select 8
#define ball 9
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;

out vec3 colorv;
out float lambert;

void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja slides 144 e 150 do documento "Aula_09_Projecoes.pdf".
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slide 189 do documento "Aula_09_Projecoes.pdf".

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slide 107 do documento "Aula_07_Transformacoes_Geometricas_3D.pdf".
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.5,1.0));
    vec4 r = -l + 2*n*dot(n,l);
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;
    vec4 p = position_world;
    vec4 v = normalize(camera_position - p);

    float U = 0.0;
    float V = 0.0;

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks ; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    vec3 Kd0;

    float ql;

    vec4 halfway;
    int useOnlyLambert = 0;

    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
        // projeção esférica EM COORDENADAS DO MODELO. Utilize como referência
        // o slide 144 do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        // A esfera que define a projeção deve estar centrada na posição
        // "bbox_center" definida abaixo.

        // Você deve utilizar:
        //   função 'length( )' : comprimento Euclidiano de um vetor
        //   função 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
        //   função 'asin( )'   : seno inverso.
        //   constante M_PI
        //   variável position_model



        // Parâmetros que definem as propriedades espectrais da superfície

        Ks = vec3(0,0,0); // Refletância especular
        Ka= vec3(0,0,0); // Refletância ambienrte
        q = 1;

        useOnlyLambert = 1;
    }
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        Kd = vec3(1.0, 1.0, 0.5);
        Ks = vec3(0, 0, 0);
        Ka = Kd/2;

        // Parâmetros que definem as propriedades espectrais da superfície
        Kd0 = vec3(1.0,1.0,1.0); // Refletância difusa
        Ks = vec3(0.1,0.1,0.12); // Refletância especular
        Ka= vec3(0,0,0); // Refletância ambiente
        q = 50;

    }
    else if ( object_id == PLANEE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.


        // Parâmetros que definem as propriedades espectrais da superfície
        Kd0 = vec3(1.0,1.0,1.0); // Refletância difusa
        Ks = vec3(0.1,0.1,0.12); // Refletância especular
        Ka= vec3(0,0,0); // Refletância ambiente
        q = 50;
    }
    else if(object_id == select)
    {
        Ks = vec3(0.01,0.01,0.05); // Refletância especular
        Ka= vec3(0.5,0.5,0.1); // Refletância ambiente
        q = 1;
    }
    else if(object_id == ball)
    {

        Kd0 = vec3(0,0,0);
        Ks = vec3(0.05,0.05,0.05); // Refletância especular
        Ka= vec3(0.1,0.1,0.1); // Refletância ambiente
        q = 0.01;
    }
    if(object_id != CUBO || object_id != BARCOE)
    {
        lambert = max(0,dot(n,l));


        //iluminação de bling phon - comentado por hora porque precisa setar todos os valores, senão fica tudo preto na cena

        // Termo difuso utilizando a lei dos cossenos de Lambert
        float lambert_diffuse_term = max(0,dot(n,l));

        halfway = normalize(v+(l-p));
        // Termo especular utilizando o modelo de iluminação de Phong
        float phong_specular_term  = pow(max(0,dot(n,halfway)),q);
        // Espectro da fonte de iluminação
        vec3 light_spectrum = vec3(1.0,1.0,1.0);

        // Espectro da luz ambiente
        vec3 ambient_light_spectrum = vec3(0.2,0.2,0.2);

        colorv = Kd0 * light_spectrum * lambert_diffuse_term
                 + Ka * ambient_light_spectrum
                 + Ks * light_spectrum * phong_specular_term;
    }

    // Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
    texcoords = texture_coefficients;
}

