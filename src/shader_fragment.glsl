#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpola��o da posi��o global e a normal de cada v�rtice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posi��o do v�rtice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

in vec4 lv;
in vec4 nv;
in vec4 rv;
in vec4 vv;
in float lambertv;

// Matrizes computadas no c�digo C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto est� sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define PLANEE  6
#define CUBO 4
#define BARCOE 5
#define seenselect 7
#define select 8
#define ball 9
#define SEEN 10


uniform int object_id;

// Par�metros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Vari�veis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;
uniform sampler2D TextureImage5;
uniform sampler2D TextureImage6;
uniform sampler2D TextureImage7;

// O valor de sa�da ("out") de um Fragment Shader � a cor final do fragmento.
out vec3 color;
in vec3 colorv;
in float lambert;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posi��o da c�mera utilizando a inversa da matriz que define o
    // sistema de coordenadas da c�mera.


    // O fragmento atual � coberto por um ponto que percente � superf�cie de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posi��o no
    // sistema de coordenadas global (World coordinates). Esta posi��o � obtida
    // atrav�s da interpola��o, feita pelo rasterizador, da posi��o de cada
    // v�rtice.


    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada v�rtice.


    // Vetor que define o sentido da c�mera em rela��o ao ponto atual.


    // Vetor que define o sentido da reflex�o especular ideal.


    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    // Par�metros que definem as propriedades espectrais da superf�cie
    vec3 Kd; // Reflet�ncia difusa
    vec3 Ks ; // Reflet�ncia especular
    vec3 Ka; // Reflet�ncia ambiente
    float q; // Expoente especular para o modelo de ilumina��o de Phong

    vec3 Kd0;

    int useOnlyLambert = 0;

    color = colorv;
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


        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 max_vec = bbox_max;
        max_vec.w = 0.0;
        float ro= length(max_vec);

        vec4 p_vec = normalize(position_model - bbox_center) * ro;
        vec4 p_linha = bbox_center + p_vec;

        float px =  p_linha.x;
        float py =  p_linha.y;
        float pz =  p_linha.z;

        float theta = atan(px,pz);
        float phi = asin(py/ro);


        U = (theta +M_PI)/(2.0*M_PI);
        V = (phi + (M_PI_2))/M_PI;

        // Par�metros que definem as propriedades espectrais da superf�cie
        Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
        color = Kd0*(lambert+0.01);
    }
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(TextureImage2, vec2(U,V)).rgb;
        color = Kd0*(lambert+0.01);

    }
    else if ( object_id == seenselect )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(TextureImage7, vec2(U,V)).rgb;
        color = Kd0*(lambert+0.01);

    }
    else if ( object_id == SEEN )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(TextureImage6, vec2(U,V)).rgb;

        color = Kd0*(lambert+0.01);
        // Par�metros que definem as propriedades espectrais da superf�cie
    }
    else if ( object_id == PLANEE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(TextureImage3, vec2(U,V)).rgb;

        color = Kd0*(lambert+0.01);
        // Par�metros que definem as propriedades espectrais da superf�cie
    }
    else if(object_id == select)
    {
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(TextureImage4, vec2(U,V)).rgb;
        color = Kd0*(lambert+0.01);
    }
    else if(object_id == CUBO)
    {

        Kd0 = vec3(1,0,0);

        // Par�metros que definem as propriedades espectrais da superf�cie
        Ks = vec3(0.25,0.25,0.25); // Reflet�ncia especular
        Ka= vec3(0,0,0); // Reflet�ncia ambiente
        q = 25;
    }
    else if(object_id == BARCOE)
    {

        Kd0 = vec3(0,0,1);

        // Par�metros que definem as propriedades espectrais da superf�cie
        Ks = vec3(0.25,0.25,0.5); // Reflet�ncia especular
        Ka= vec3(0,0,0); // Reflet�ncia ambiente
        q = 25;
    }
    if(object_id == CUBO || object_id == BARCOE)
    {
        vec4 n = normalize(normal);

        vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));
        vec4 r = -l + 2*n*dot(n,l);
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 v = normalize(camera_position - p);
        float lambert = max(0,dot(nv,lv));



        //ilumina��o de bling phon - comentado por hora porque precisa setar todos os valores, sen�o fica tudo preto na cena

        // Termo difuso utilizando a lei dos cossenos de Lambert
        float lambert_diffuse_term = max(0,dot(n,l));

        // Termo especular utilizando o modelo de ilumina��o de Phong
        float phong_specular_term  = pow(max(0,dot(r,v)),q);

        // Espectro da fonte de ilumina��o
        vec3 light_spectrum = vec3(1.0,1.0,1.0);

        // Espectro da luz ambiente
        vec3 ambient_light_spectrum = vec3(0.2,0.2,0.2);

        color = Kd0 * light_spectrum * lambert_diffuse_term
                 + Ka * ambient_light_spectrum
                 + Ks * light_spectrum * phong_specular_term;
    }
    // Obtemos a reflet�ncia difusa a partir da leitura da imagem TextureImage0


    // Equa��o de Ilumina��o


    ///if (useOnlyLambert == 1)
    //{
      //  color = Kd0*(lambert+0.01);
    //}

    // Cor final com corre��o gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color = pow(color, vec3(1.0,1.0,1.0)/2.2);
}

