#include "CShader.h"
#include "Engine.h"

namespace irr
{


namespace scene
{

static void defaultOnRender(IShader &shader)
{

}
static void defaultOnLoad(IShader &shader)
{

}


IShader::IShader(const char* vertexPath, const char* fragmentPath,bool isFile)
{

    OnRender=defaultOnRender;
    OnLoad=defaultOnLoad;
    isLoad=false;

	// 2. compile shaders
	unsigned int vertex, fragment;

	if (isFile)
	{
	 char* vShaderCode  =  core::LoadFileText(vertexPath);
	 char * fShaderCode = core::LoadFileText(fragmentPath);

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	free(vShaderCode);
  	free(fShaderCode);
	   vShaderCode=NULL;
   fShaderCode=NULL;


	} else
	{
			// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexPath, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentPath, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	}

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
   //      SDL_Log("SHADER [%d] Created \n",ID);
         LoadDefaults();
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

	void IShader::FromString(const char* vertexCode, const char* fragmentCode)
{
	// 2. compile shaders
	unsigned int vertex, fragment;


			// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");



	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	//	 SDL_Log("SHADER [%d] Created \n",ID);
		 LoadDefaults();
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
void IShader::Load(const char* vertexPath, const char* fragmentPath)
{
	// 2. compile shaders
	unsigned int vertex, fragment;


	 char* vShaderCode  =  core::LoadFileText(vertexPath);
	 char * fShaderCode = core::LoadFileText(fragmentPath);

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

   free(vShaderCode);
   free(fShaderCode);
   vShaderCode=NULL;
   fShaderCode=NULL;




	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
//	SDL_Log("SHADER [%d] Created \n",ID);
	LoadDefaults();
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);


}
/*
IShader::IShader(const char* vertexPath, const char* fragmentPath,const char* geometryPath,bool isFile)
{
	// 2. compile shaders
	unsigned int vertex, fragment,geometry;

	if (isFile)
	{
	 char* vShaderCode  =  LoadFileText(vertexPath);
	 char * fShaderCode = LoadFileText(fragmentPath);
	 char * fGeometryCode = LoadFileText(geometryPath);

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");


	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &fGeometryCode, NULL);
	glCompileShader(geometry);
	checkCompileErrors(geometry, "GEOMETRY");


   free(vShaderCode);
   free(fShaderCode);
   free(fGeometryCode);
   vShaderCode=NULL;
   fShaderCode=NULL;
   fGeometryCode=NULL;

	} else
	{
			// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexPath, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentPath, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &geometryPath, NULL);
	glCompileShader(geometry);
	checkCompileErrors(geometry, "GEOMETRY");

	}

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
		 SDL_Log("SHADER [%d] Created \n",ID);
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

void IShader::FromString(const char* vertexCode, const char* fragmentCode,const char* geometryCode)
{
	// 2. compile shaders
	unsigned int vertex, fragment;


			// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

     unsigned int geometry;

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &geometryCode, NULL);
	glCompileShader(geometry);
	checkCompileErrors(geometry, "GEOMETRY");



	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
		 SDL_Log("SHADER [%d] Created \n",ID);
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}
void IShader::Load(const char* vertexPath, const char* fragmentPath,const char* geometryPath)
{
	// 2. compile shaders
	unsigned int vertex, fragment;


	 char* vShaderCode    = Utils::LoadFileText(vertexPath);
	 char * fShaderCode   = Utils::LoadFileText(fragmentPath);
	 char * fGeometryCode = Utils::LoadFileText(geometryPath);


	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment IShader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	  unsigned int geometry;

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &fGeometryCode, NULL);
	glCompileShader(geometry);
	checkCompileErrors(geometry, "GEOMETRY");

   free(vShaderCode);
   free(fShaderCode);
   free(fGeometryCode);
   vShaderCode=NULL;
   fShaderCode=NULL;
   fGeometryCode=NULL;




	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	 SDL_Log("SHADER [%d] Created \n",ID);
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}
*/
// activate the shader
// ------------------------------------------------------------------------
void IShader::Unload()
{
	glDeleteProgram(ID);
	SDL_Log("SHADER: [ID %i] Unloaded shader program data", ID);

}
bool IShader::findUniform(const std::string name)const
{
   std::map<std::string, int>::const_iterator it = uniforms.begin();
    while(it != uniforms.end())
    {
         if (strcmp(it->first.c_str(),name.c_str())==0)
         {
           return true;
         }
        it++;
    }
    return false;
}
int  IShader::getUniform(const std::string name)const
{

   std::map<std::string, int>::const_iterator it = uniforms.begin();
    while(it != uniforms.end())
    {
         if (strcmp(it->first.c_str(),name.c_str())==0)
         {
           return it->second;
         }

        it++;
    }
return -1;
}
int IShader::getUniformLocation(const std::string &uniformName) const
{
    int location = -1;
    //location = glGetUniformLocation(ID, uniformName.c_str());
    location =getUniform(uniformName);//uniforms[uniformName];

    if (location == -1)
     SDL_Log( "SHADER: [ID %i] Failed to find shader uniform: %s", ID, uniformName.c_str());
    //App::getInstance()->ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"SHADER Error",TextFormat("SHADER Failed to find shader uniform: %s)",uniformName));

  //  else SDL_Log( "SHADER: [ID %i] IShader uniform (%s) set at location: %i", id, uniformName, location);
    return location;
}

int IShader::getAttribLocation( const std::string &attribName) const
{
    int location = -1;
    location = glGetAttribLocation(ID, attribName.c_str());

    if (location == -1)
    SDL_Log( "SHADER: [ID %i] Failed to find shader attribute: %s", ID, attribName.c_str());
    //App::getInstance()->ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"SHADER Error",TextFormat("SHADER Failed to find shader attribute: %s)",attribName));
  //  else SDL_Log( "SHADER: [ID %i] IShader attribute (%s) set at location: %i", id, attribName, location);
    return location;
}
bool IShader::addUniform(const std::string name)
{


    int location = -1;
    location = glGetUniformLocation(ID, name.c_str());

    if (location == -1)
    {
      SDL_Log( "SHADER: [ID %i] Failed to find shader uniform: %s", ID, name.c_str());
      //ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"SHADER Error",TextFormat("SHADER Failed to find shader uniform: %s)",name));
      return false;
    }

SDL_Log( "SHADER: [ID %i] shader uniform (%s) set at location: %i", ID, name.c_str(), location);
uniforms.insert(std::make_pair(name, location));

return true;
}
bool IShader::addAttribute(const std::string name)
{
   int location = -1;
    location = glGetAttribLocation(ID, name.c_str());

    if (location == -1)
    {
      SDL_Log( "SHADER: [ID %i] Failed to find shader attribute: %s", ID, name.c_str());
      //ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"SHADER Error",TextFormat("SHADER Failed to find shader attribute: %s)",name));
     return false;
    }

SDL_Log( "SHADER: [ID %i] shader attribute (%s) set at location: %i", ID, name.c_str(), location);
attributes.insert(std::make_pair(name, location));

return true;
}
void IShader::printData()
{

   SDL_Log("[SHADER]  Id(%d) Num Attributes(%d)  Num Uniforms (%d)",ID,this->numAttributes,this->numUniforms);


   {
    std::map<std::string, int>::iterator it = attributes.begin();
    while(it != attributes.end())
    {
        SDL_Log("[SHADER]  Id(%d)  attribute  index(%d) name(%s)",ID,it->second, it->first.c_str());

        it++;
    }
   }
   {
    std::map<std::string, int>::iterator it = uniforms.begin();
    while(it != uniforms.end())
    {
        SDL_Log("[SHADER]  Id(%d) uniform index(%d) name(%s)",ID,it->second, it->first.c_str());

        it++;
    }
    }
}
void IShader::LoadDefaults()
{
    GLint numActiveAttribs = 0;
	GLint maxAttribNameLength = 0;
	glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
	glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
	numAttributes=numActiveAttribs;
	for(GLint attrib = 0; attrib < numActiveAttribs; ++attrib)
	{
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;
		glGetActiveAttrib(ID, attrib,  sizeof(name) - 1, &namelen, &num, &type, name);
		name[namelen] = 0;
        addAttribute(std::string((char*)&name[0]));
		glBindAttribLocation(ID, attrib, (char*)&name[0]);
 	 // TRACELOGD("SHADER: [ID %i] Active attribute (%s) set at location: %i", shader.id, name,attrib);
	}


    // Get available shader uniforms
    int uniformCount = -1;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformCount);

    numUniforms=uniformCount;

    for (int i = 0; i < uniformCount; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        GLenum type = GL_ZERO;


        glGetActiveUniform(ID, i, sizeof(name) - 1, &namelen, &num, &type, name);

        name[namelen] = 0;

        addUniform(std::string((char*)&name[0]));

     //   SDL_Log("SHADER: [ID %i] Active uniform (%s) set at location: %i", shader.id, name, glGetUniformLocation(shader.id, name));
    }

}

void IShader::Use()
{
	glUseProgram(ID);

	//Engine::Instance()->setProgram(ID);

	if (!isLoad)
	{
	  if (OnLoad)
	  {
	    OnLoad(*this);
	  }
	  isLoad=true;
	}


     if (OnRender)
     {
       OnRender(*this);
     }

}
void IShader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			SDL_Log("ERROR::SHADER_COMPILATION_ERROR of type: %s % error : %s ",  type .c_str(), infoLog );
		  scene::Engine::Instance()->ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Compile Error",infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			SDL_Log("ERROR::PROGRAM_LINKING_ERROR of type: %s % error : %s ",  type .c_str(), infoLog );
			scene::Engine::Instance()->ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"LINK Error",infoLog);
		}
	}
}



}
}
