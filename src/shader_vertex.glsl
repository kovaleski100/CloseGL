#version 330 core

// Atributos de v�rtice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a fun��o BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no c�digo C++ e enviadas para a GPU
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

// Par�metros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Atributos de v�rtice que ser�o gerados como sa�da ("out") pelo Vertex Shader.
// ** Estes ser�o interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais ser�o recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;

out vec3 colorv;
out float lambert;

void main()
{
    // A vari�vel gl_Position define a posi��o final de cada v�rtice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estar� entre -1 e 1 ap�s divis�o por w.
    // Veja slides 144 e 150 do documento "Aula_09_Projecoes.pdf".
    //
    // O c�digo em "main.cpp" define os v�rtices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // opera��es de modelagem, defini��o da c�mera, e proje��o, para computar
    // as coordenadas finais em NDC (vari�vel gl_Position). Ap�s a execu��o
    // deste Vertex Shader, a placa de v�deo (GPU) far� a divis�o por W. Veja
    // slide 189 do documento "Aula_09_Projecoes.pdf".

    gl_Position = projection * view * model * model_coefficients;

    // Como as vari�veis acima  (tipo vec4) s�o vetores com 4 coeficientes,
    // tamb�m � poss�vel acessar e modificar cada coeficiente de maneira
    // independente. Esses s�o indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto �, 'x' � o primeiro coeficiente, 'y' � o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos v�rtices que ser�o interpolados pelo
    // rasterizador para gerar atributos �nicos para cada fragmento gerado.

    // Posi��o do v�rtice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posi��o do v�rtice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do v�rtice atual no sistema de coordenadas global (World).
    // Veja slide 107 do documento "Aula_07_Transformacoes_Geometricas_3D.pdf".
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em rela��o ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.5,1.0));
    vec4 r = -l + 2*n*dot(n,l);
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;
    vec4 p = position_world;
    vec4 v = normalize(camera_position - p);

    float U = 0.0;
    float V = 0.0;

    // Par�metros que definem as propriedades espectrais da superf�cie
    vec3 Kd; // Reflet�ncia difusa
    vec3 Ks ; // Reflet�ncia especular
    vec3 Ka; // Reflet�ncia ambiente
    float q; // Expoente especular para o modelo de ilumina��o de Phong

    vec3 Kd0;

    float ql;

    vec4 halfway;
    int useOnlyLambert = 0;

    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
        // proje��o esf�rica EM COORDENADAS DO MODELO. Utilize como refer�ncia
        // o slide 144 do documento "Aula_20_e_21_Mapeamento_de_Texturas.pdf".
        // A esfera que define a proje��o deve estar centrada na posi��o
        // "bbox_center" definida abaixo.

        // Voc� deve utilizar:
        //   fun��o 'length( )' : comprimento Euclidiano de um vetor
        //   fun��o 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
        //   fun��o 'asin( )'   : seno inverso.
        //   constante M_PI
        //   vari�vel position_model



        // Par�metros que definem as propriedades espectrais da superf�cie

        Ks = vec3(0,0,0); // Reflet�ncia especular
        Ka= vec3(0,0,0); // Reflet�ncia ambienrte
        q = 1;

        useOnlyLambert = 1;
    }
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        Kd = vec3(1.0, 1.0, 0.5);
        Ks = vec3(0, 0, 0);
        Ka = Kd/2;

        // Par�metros que definem as propriedades espectrais da superf�cie
        Kd0 = vec3(1.0,1.0,1.0); // Reflet�ncia difusa
        Ks = vec3(0.1,0.1,0.12); // Reflet�ncia especular
        Ka= vec3(0,0,0); // Reflet�ncia ambiente
        q = 50;

    }
    else if ( object_id == PLANEE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.


        // Par�metros que definem as propriedades espectrais da superf�cie
        Kd0 = vec3(1.0,1.0,1.0); // Reflet�ncia difusa
        Ks = vec3(0.1,0.1,0.12); // Reflet�ncia especular
        Ka= vec3(0,0,0); // Reflet�ncia ambiente
        q = 50;
    }
    else if(object_id == select)
    {
        Ks = vec3(0.01,0.01,0.05); // Reflet�ncia especular
        Ka= vec3(0.5,0.5,0.1); // Reflet�ncia ambiente
        q = 1;
    }
    else if(object_id == ball)
    {

        Kd0 = vec3(0,0,0);
        Ks = vec3(0.05,0.05,0.05); // Reflet�ncia especular
        Ka= vec3(0.1,0.1,0.1); // Reflet�ncia ambiente
        q = 0.01;
    }
    if(object_id != CUBO || object_id != BARCOE)
    {
        lambert = max(0,dot(n,l));


        //ilumina��o de bling phon - comentado por hora porque precisa setar todos os valores, sen�o fica tudo preto na cena

        // Termo difuso utilizando a lei dos cossenos de Lambert
        float lambert_diffuse_term = max(0,dot(n,l));

        halfway = normalize(v+(l-p));
        // Termo especular utilizando o modelo de ilumina��o de Phong
        float phong_specular_term  = pow(max(0,dot(n,halfway)),q);
        // Espectro da fonte de ilumina��o
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

