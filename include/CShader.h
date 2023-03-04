#ifndef CIShader_H
#define CIShader_H

#include <functional>

#include "CImage.h"
#include "COpenGLTexture.h"
#include "os.h"
#include "IrrCompileConfig.h"
#include "aabbox3d.h"
#include "CMeshBuffer.h"
#include "coreutil.h"

#include "dimension2d.h"
#include "IEngineEnums.h"


#include "fast_atof.h"
#include "heapsort.h"
#include "IAnimatedMesh.h"
#include "IFileList.h"
#include "IFileSystem.h"
#include "CFileSystem.h"
#include "IGeometryCreator.h"
#include "IImage.h"
#include "IIndexBuffer.h"
#include "ILogger.h"

#include "IReferenceCounted.h"
#include "irrArray.h"

#include "irrList.h"
#include "irrMap.h"
#include "irrMath.h"
#include "irrString.h"
#include "irrTypes.h"
#include "path.h"

#include "ISkinnedMesh.h"
#include "ITexture.h"

#include "line2d.h"
#include "line3d.h"
#include "matrix4.h"
#include "plane3d.h"
#include "position2d.h"
#include "quaternion.h"
#include "rect.h"
#include "S3DVertex.h"
#include "SColor.h"
#include "SMaterial.h"
#include "triangle3d.h"
#include "vector2d.h"
#include "vector3d.h"
namespace irr
{


namespace scene
{

class IShader
{
public:
	unsigned int ID;
	std::function<void(IShader&)>OnRender;
	std::function<void(IShader&)>OnLoad;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	IShader(){ID=0;};

	IShader(const char* vertexPath, const char* fragmentPath,bool isFile=true);
	virtual ~IShader(){};

	//IShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath,bool isFile=true);
	void FromString(const char* vertexCode, const char* fragmentCode);
	//void FromString(const char* vertexCode, const char* fragmentCode, const char* geometryCode);
	void Load(const char* vertexPath, const char* fragmentPath);
	//void Load(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	void printData();
	void Unload();
	void Use();
	// utility uniform functions

    bool addUniform(const std::string name)  ;
    bool addAttribute(const std::string name) ;

    bool findUniform(const std::string name) const ;
    int  getUniform(const std::string name) const ;


    void setOnRender(std::function<void(IShader&)>func )
     {
      OnRender = func;
     }

     void setOnLoad(std::function<void(IShader&)>func )
     {
      OnLoad = func;
     }


    bool operator ==(const IShader&      other) const { return ID == other.ID; }
    bool operator !=(const IShader&      other) const { return ID != other.ID; }
    // ------------------------------------------------------------------------

    int getUniformLocation(const std::string &uniformName) const;
    int getAttribLocation(const std::string &attribName) const;


	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(getUniformLocation(name), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(getUniformLocation(name), value);
	}
	// ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
	{
		glUniform1f(getUniformLocation(name), value);
    }

     void setVec2(const std::string &name, const core::vector2df &value) const
	{
		glUniform2f(getUniformLocation(name),  value.X,value.Y);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
		glUniform2f(getUniformLocation(name), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const core::vector3df  &value) const
    {
		glUniform3f(getUniformLocation(name),  value.X,value.Y,value.Z);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
		glUniform3f(getUniformLocation(name), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const core::quaternion &value) const
    {
		glUniform4f(getUniformLocation(name), value.X,value.Y,value.Z,value.W);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(getUniformLocation(name), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const float* mat) const
    {
		glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, mat);
    }
    // ------------------------------------------------------------------------
	void setMat3(const std::string &name, const float* mat) const
    {
		glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, mat);
    }
    void setMat3(const std::string &name,  const core::matrix3 &mat) const
    {
		glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, mat.pointer());
    }
    // ------------------------------------------------------------------------

	void setMat4(const std::string &name, const core::matrix4 &mat) const
    {
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat.pointer());
	}

	void setMat(const std::string &name, const float* mat) const
    {
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat);
	}
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
	void checkCompileErrors(unsigned int shader, std::string type);
	void LoadDefaults();
    int numAttributes;
    int numUniforms;
    bool isLoad;


     std::map<std::string, int> uniforms;
     std::map<std::string, int> attributes;

};

}
}

#endif // CIShader_H
