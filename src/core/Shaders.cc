


void createlLightTextureDetailShader()
{



const char* vertexSource = GLSL(

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
attribute vec2 inTexCoord1;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3  CameraPosition;

varying vec2 varTextureCoord0;
varying vec2 varTextureCoord1;
varying vec4 varVertexColor;
varying  vec3 vLighting;
varying float FogAmount;


mat3 transpose(mat3 matrix)
{
    vec3 row0 = matrix[0];
    vec3 row1 = matrix[1];
    vec3 row2 = matrix[2];
    mat3 result = mat3(
        vec3(row0.x, row1.x, row2.x),
        vec3(row0.y, row1.y, row2.y),
        vec3(row0.z, row1.z, row2.z)
    );
    return result;
}

float det(mat2 matrix)
{
    return matrix[0].x * matrix[1].y - matrix[0].y * matrix[1].x;
}

mat3 inverse(mat3 matrix) {
    vec3 row0 = matrix[0];
    vec3 row1 = matrix[1];
    vec3 row2 = matrix[2];

    vec3 minors0 = vec3(
        det(mat2(row1.y, row1.z, row2.y, row2.z)),
        det(mat2(row1.z, row1.x, row2.z, row2.x)),
        det(mat2(row1.x, row1.y, row2.x, row2.y))
    );
    vec3 minors1 = vec3(
        det(mat2(row2.y, row2.z, row0.y, row0.z)),
        det(mat2(row2.z, row2.x, row0.z, row0.x)),
        det(mat2(row2.x, row2.y, row0.x, row0.y))
    );
    vec3 minors2 = vec3(
        det(mat2(row0.y, row0.z, row1.y, row1.z)),
        det(mat2(row0.z, row0.x, row1.z, row1.x)),
        det(mat2(row0.x, row0.y, row1.x, row1.y))
    );

    mat3 adj = transpose(mat3(minors0, minors1, minors2));

    return (1.0 / dot(row0, minors0)) * adj;
}

void main()
{

	varTextureCoord0 = inTexCoord0;
	varTextureCoord1 = inTexCoord1;
	varVertexColor   = inVertexColor;

gl_Position = projection * view * model *   vec4(inVertexPosition,1.0);


       vec3 ambientLight = vec3(1.0, 1.0, 1.0);
       vec3 directionalLightColor = vec3(1, 1, 1);
       //vec3 directionalVector = normalize(vec3(0, -1, 0.75));//vec3(0.85, 0.8, 0.75));
       vec3 directionalVector = normalize(vec3(0.85, 0.8, 0.75));

       mat4 normalMatrix = view * model;

       vec3  uNormalMatrix = mat3(transpose(inverse(mat3(normalMatrix)))) * inVertexNormal;


       float directional = max(dot(uNormalMatrix.xyz, directionalVector), 0.0);
       vLighting = ambientLight + (directionalLightColor * directional);



        float FogStart=1.0;
        float FogRange=4000.0;

//        vec3 view_pos = getTranslation(view);


        vec3 worldPosition = ( model * vec4(inVertexPosition,0.0) ).xyz;
        FogAmount = clamp((distance(worldPosition, CameraPosition) -FogStart ) / FogRange,0.0,1.0);
        //FogAmount = clamp((distance(worldPosition, view_pos) -FogStart ) / FogRange,0.0,1.0);


}

			);

const char *fragmentSource = GLSL(
precision highp float;

uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;
uniform vec3  CameraPosition;

varying vec2 varTextureCoord0;
varying vec2 varTextureCoord1;

varying vec4 varVertexColor;
varying  vec3 vLighting;
varying float FogAmount;



vec4 render2LayerSolid()
{
	float BlendFactor = varVertexColor.a;

	vec4 Texel0 = texture2D(uTextureUnit0, varTextureCoord0);
	vec4 Texel1 = texture2D(uTextureUnit1, varTextureCoord1);

	vec4 Color = Texel0 * BlendFactor + Texel1 * (1.0 - BlendFactor);

	return Color;
}

vec4 renderLightMap()
{

    vec4 Texel0 = texture2D(uTextureUnit0, varTextureCoord0);
	vec4 Texel1 = texture2D(uTextureUnit1, varTextureCoord1);



	vec4 Color = Texel0 * Texel1 * 4.0;
	Color.a = Texel0.a * Texel0.a;

	return Color;
}

vec4 renderDetailMap()
{
    vec4 Texel0 = texture2D(uTextureUnit0, varTextureCoord0);
	vec4 Texel1 = texture2D(uTextureUnit1, varTextureCoord1);

    vec4 Color = Texel0 * Texel1;


	return Color;
}

void main()
{

   vec4 FogColor= vec4(0.1,0.1,0.2,0.0);

       if (FogAmount==1.0)
       {
        gl_FragColor = vec4(FogColor.rgb,1.0);
        return;
       }


    vec4 texelColor = renderDetailMap();
    texelColor *= vec4(vLighting,1.0);
   // gl_FragColor = texelColor;



    vec4 Color = vec4(texelColor.rgb * vLighting, texelColor.a);



    vec4 FinalColor=vec4(1.0,1.0,1.0,1.0);
    FinalColor.rgb = mix(Color.rgb,FogColor.rgb,FogAmount);
    FinalColor.a = texelColor.a;


    gl_FragColor = FinalColor;


}

);



//Assets::LoadTexture("media/cube_diffuse.png","cube_diffuse");

IShader *s2 =scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"LightDetailShader");
s2->Use();
s2->setInt("uTextureUnit0",0);
s2->setInt("uTextureUnit1",1);

}



void createlLightTextureShader()
{



const char* vertexSource = GLSL(

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3  CameraPosition;

varying vec2 varTextureCoord0;
varying vec4 varVertexColor;
varying  vec3 vLighting;
varying float FogAmount;


vec3 getTranslation(mat4 matrix)
{
    vec4 row2 = matrix[3];

    return vec3(row2.x,row2.y,row2.z);
}

mat3 transpose(mat3 matrix)
{
    vec3 row0 = matrix[0];
    vec3 row1 = matrix[1];
    vec3 row2 = matrix[2];
    mat3 result = mat3(
        vec3(row0.x, row1.x, row2.x),
        vec3(row0.y, row1.y, row2.y),
        vec3(row0.z, row1.z, row2.z)
    );
    return result;
}

float det(mat2 matrix)
{
    return matrix[0].x * matrix[1].y - matrix[0].y * matrix[1].x;
}

mat3 inverse(mat3 matrix) {
    vec3 row0 = matrix[0];
    vec3 row1 = matrix[1];
    vec3 row2 = matrix[2];

    vec3 minors0 = vec3(
        det(mat2(row1.y, row1.z, row2.y, row2.z)),
        det(mat2(row1.z, row1.x, row2.z, row2.x)),
        det(mat2(row1.x, row1.y, row2.x, row2.y))
    );
    vec3 minors1 = vec3(
        det(mat2(row2.y, row2.z, row0.y, row0.z)),
        det(mat2(row2.z, row2.x, row0.z, row0.x)),
        det(mat2(row2.x, row2.y, row0.x, row0.y))
    );
    vec3 minors2 = vec3(
        det(mat2(row0.y, row0.z, row1.y, row1.z)),
        det(mat2(row0.z, row0.x, row1.z, row1.x)),
        det(mat2(row0.x, row0.y, row1.x, row1.y))
    );

    mat3 adj = transpose(mat3(minors0, minors1, minors2));

    return (1.0 / dot(row0, minors0)) * adj;
}

void main()
{

	varTextureCoord0 = inTexCoord0;
	varVertexColor   = inVertexColor;





       vec3 ambientLight = vec3(1.0, 1.0, 1.0);
       vec3 directionalLightColor = vec3(1, 1, 1);
       //vec3 directionalVector = normalize(vec3(0, -1, 0.75));//vec3(0.85, 0.8, 0.75));
       vec3 directionalVector = normalize(vec3(0.85, 0.8, 0.75));

       mat4 normalMatrix = view * model;

       vec3  uNormalMatrix = mat3(transpose(inverse(mat3(normalMatrix)))) * inVertexNormal;


       float directional = max(dot(uNormalMatrix.xyz, directionalVector), 0.0);
       vLighting = ambientLight + (directionalLightColor * directional);

        float FogStart=1.0;
        float FogRange=4000.0;

        vec3 view_pos = getTranslation(view);


        vec3 worldPosition = ( model * vec4(inVertexPosition,0.0) ).xyz;
        FogAmount = clamp((distance(worldPosition, CameraPosition) -FogStart ) / FogRange,0.0,1.0);
        //FogAmount = clamp((distance(worldPosition, view_pos) -FogStart ) / FogRange,0.0,1.0);


       gl_Position = projection * view * model *   vec4(inVertexPosition,1.0);
}

			);

const char *fragmentSource = GLSL(
precision highp float;

uniform sampler2D uTextureUnit;


varying vec2 varTextureCoord0;
varying vec4 varVertexColor;
varying  vec3 vLighting;
varying float FogAmount;


vec4 renderSolid()
{
	//vec4 Color = varVertexColor;
	vec4 Color = vec4(1.0,1.0,1.0,1.0);
	Color *= texture2D(uTextureUnit, varTextureCoord0);
	Color.a = 1.0;
	return Color;
}


void main()
{
   vec4 FogColor= vec4(0.1,0.1,0.2,0.0);

   if (FogAmount==1.0)
   {
    gl_FragColor = vec4(FogColor.rgb,1.0);
    return;
   }


    vec4 texelColor = renderSolid();// texture2D(uTextureUnit, varTextureCoord0) *varVertexColor ;
    //gl_FragColor = vec4(texelColor.rgb * vLighting, texelColor.a);

    vec4 Color = vec4(texelColor.rgb * vLighting, texelColor.a);


    vec4 FinalColor=vec4(1.0,1.0,1.0,1.0);
    FinalColor.rgb = mix(Color.rgb,FogColor.rgb,FogAmount);
    FinalColor.a = texelColor.a;


    gl_FragColor = FinalColor;


   //gl_FragColor = texelColor;
}

);



//Assets::LoadTexture("media/cube_diffuse.png","cube_diffuse");

IShader *s2 =scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"LightShader");
s2->Use();
s2->setInt("uTextureUnit",0);

}

void createTextureShader()
{



const char* vertexSource = GLSL(

attribute vec3 inVertexPosition;
attribute vec2 inTexCoord0;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec2 varTextureCoord0;


void main()
{

	varTextureCoord0 = inTexCoord0;
    gl_Position = projection * view * model *   vec4(inVertexPosition,1.0);

}

			);

const char *fragmentSource = GLSL(
precision highp float;

uniform sampler2D uTextureUnit;
varying vec2 varTextureCoord0;
varying vec4 varVertexColor;



void main()
{
    vec4 texelColor = texture2D(uTextureUnit, varTextureCoord0);
    gl_FragColor = texelColor;
}

);



//Assets::LoadTexture("media/cube_diffuse.png","cube_diffuse");

IShader *s2 =scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"Texture");
s2->Use();
s2->setInt("uTextureUnit",0);

}

void createSolidShader()
{



const char* vertexSource = GLSL(

attribute vec3 a_position;
attribute vec3 a_color;
varying vec3 v_color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
v_color=a_color;
gl_Position = projection * view * model *   vec4(a_position,1.0);
}

			);

const char *fragmentSource = GLSL(
precision highp float;
varying vec3 v_color;
void main()
{
    gl_FragColor = vec4(v_color,1.0);
}

);

scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"Solid");

}

void createPointsShader()
{


const char* vertexSource = GLSL(

attribute vec3 a_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{

gl_Position = projection * view * model *   vec4(a_position,1.0);
}

			);

const char *fragmentSource = GLSL(
precision highp float;

void main()
{
    gl_FragColor = vec4(0.04, 0.28, 0.26, 1.0);
}

);

scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"Points");

}


void createScreenShader()
{
const char* vertexSource = GLSL(

attribute vec3 a_position;
//attribute vec4 a_color;
//varying vec4 v_color;

void main()
{
//v_color=a_color;
gl_Position =  vec4(a_position,1.0);
}

			);

const char *fragmentSource = GLSL(
precision highp float;
//varying vec4 v_color;

void main()
{
  //  gl_FragColor = v_color;//vec4(1.0f, 0.5f, 0.2f, 0.1f);
    gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 0.5f);
}

);

scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"screenquad");


}


void createStencilShader()
{
const char* vertexSource = GLSL(

attribute vec2 a_position;
void main()
{
gl_Position =  vec4(a_position,0.0,1.0);
}

);

const char *fragmentSource = GLSL(
precision highp float;
void main()
{
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.4);
}

);

scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"Stencil");


}



void createSkyBoxShader()
{
const char* vertexSource = GLSL3(


layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
  //  vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    //vec4 pos = projection * mat4(view) * vec4(aPos, 1.0);
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
);

const char *fragmentSource = GLSL3(
precision highp float;
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}

);

scene::IShader* s=scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"SkyBox");
s->Use();
s->setInt("skybox",0);


}



void createBatchShader()
{
const char* vertexSource = GLSL3(


in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;

uniform mat4 mvp;

void main()
{
   fragTexCoord = vertexTexCoord;
   fragColor = vertexColor;
   gl_Position = mvp *  vec4(vertexPosition, 1.0);
}

);

const char *fragmentSource = GLSL3(
precision highp float;
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;
uniform sampler2D texture0;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    finalColor = texelColor * fragColor;
}

);

scene::IShader* s=scene::Engine::Instance()->loadShader(vertexSource,fragmentSource,"Batch");
s->Use();
s->setInt("texture0",0);


}

