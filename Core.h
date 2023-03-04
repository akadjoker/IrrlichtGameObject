#ifndef CORE_H
#define CORE_H
#include "Utils.h"

#include "irrlicht.h"

using namespace irr;
using namespace scene;
using namespace video;



class ScreenQuad
{
  public:
  ScreenQuad()
  {
	 quadVAO=0;
     quadVBO=0;
  }
  void Init()
  {
			float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
  }
  void Render()
  {
     glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
  }
  void Render(float x, float y, float s)
  {
    	float quadVertices[] = {
			// positions        // texture Coords
			-s+x,  s+y,  0.0f, 1.0f,
			-s+x, -s+y,  0.0f, 0.0f,
			 s+x,  s+y,  1.0f, 1.0f,
			 s+x, -s+y,  1.0f, 0.0f,
		};

	if (quadVAO==0)
	{

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);


	}
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), &quadVertices);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
  }

  void Unload()
  {
      glDeleteVertexArrays(1, &quadVAO);
	  glDeleteBuffers(1, &quadVBO);
  }

private:
unsigned int quadVAO;
unsigned int quadVBO;

};
class RenderTexture
{
	public:
	void Init(int width, int height)
	{
	  this->width = width;
	  this->height = height;

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, width,height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	//to
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,depthTexture, 0);

	// No color output in the bound framebuffer, only depth.

	glDrawBuffers(1, GL_NONE);
    glReadBuffer(GL_NONE);//?

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		SDL_Log("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Begin()
	{

	     glViewport(0, 0, width, height);
		 glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//		 glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
		 glClear(GL_DEPTH_BUFFER_BIT);

	}
	void End()
	{
		 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Unload()
	{

		glDeleteTextures (GL_TEXTURE_2D, &depthTexture);
   	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers (1, &framebuffer);
	}

	void Use(int layer=0)
	{
	   glActiveTexture(GL_TEXTURE0+layer);
	   glBindTexture(GL_TEXTURE_2D, depthTexture);
	}
	unsigned int getDepthMap(){return depthTexture;}

	private:

	unsigned int framebuffer;
	unsigned int depthTexture;
	int width;
	int height;
};

class RenderTarget
{
	public:
	void Init(int width, int height)
	{
	  this->width = width;
	  this->height = height;



	  // framebuffer configuration
	// -------------------------
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE); */


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        SDL_Log("ERROR::FRAMEBUFFER:: Framebuffer is not complete!" );
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Begin()
	{
		 glViewport(0, 0, width, height);
		 glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//		 glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
		 glClear(GL_DEPTH_BUFFER_BIT);
	}
	void End()
	{
		 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Unload()
	{

		glDeleteTextures (GL_TEXTURE_2D, &textureColorbuffer);
		glDeleteRenderbuffers(1,&rbo);
   	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers (1, &framebuffer);
	}

	void Use(int layer=0)
	{
	   glActiveTexture(GL_TEXTURE0+layer);
	   glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	}
	unsigned int getDepthMap(){return textureColorbuffer;}

	private:

	unsigned int framebuffer;
	unsigned int rbo;
	unsigned int textureColorbuffer;
	int width;
	int height;

};

class RenderDepth
{
	public:
	void Init(int width, int height,bool cubeMap=false)
	{
	  this->width = width;
	  this->height = height;
	  isCubeMap=cubeMap;

	glGenFramebuffers(1, &framebuffer);

	// create depth texture
	if(cubeMap)
	{
	 InitCubeMap();
     	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);

	}else
	{
	 Init2D();
		// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    }



    glDrawBuffers(0,GL_NONE);
    glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void Begin()
	{
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void End()
	{
	   glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Unload()
	{


	 if (isCubeMap)
	 {
		glDeleteTextures (GL_TEXTURE_CUBE_MAP, &depthMap);
	 }else
	 {
		glDeleteTextures (GL_TEXTURE_2D, &depthMap);
	 }

   	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers (1, &framebuffer);
	}


	void Use(int layer=0)
	{
	 glActiveTexture(GL_TEXTURE0+layer);
	 if (isCubeMap)
	 {
	   glBindTexture (GL_TEXTURE_CUBE_MAP, depthMap);
	 }else
	 {
	   glBindTexture(GL_TEXTURE_2D, depthMap);
	 }

	}

	unsigned int getDepthMap(){return depthMap;}

	private:
	  void Init2D()
	  {
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

//		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	  }

	  void InitCubeMap()
	  {
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
		  for (unsigned int i = 0; i < 6; ++i)
		  {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		  }

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	  }

		unsigned int framebuffer;
		unsigned int depthMap;
		int width;
		int height;
		bool isCubeMap;
};


#endif // CORE_H
