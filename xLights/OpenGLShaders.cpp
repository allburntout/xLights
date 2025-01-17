#include "wx/wx.h"

#ifndef __WXMAC__
#include <GL/gl.h>
#ifdef _MSC_VER
#include "GL\glext.h"
#else
#include <GL/glext.h>
#endif

#include "xlGLCanvas.h"

extern PFNGLCREATESHADERPROC      glCreateShader;
extern PFNGLSHADERSOURCEPROC      glShaderSource;
extern PFNGLCOMPILESHADERPROC     glCompileShader;
extern PFNGLGETSHADERIVPROC       glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC     glCreateProgram;
extern PFNGLATTACHSHADERPROC      glAttachShader;
extern PFNGLLINKPROGRAMPROC       glLinkProgram;
extern PFNGLDETACHSHADERPROC      glDetachShader;
extern PFNGLDELETESHADERPROC      glDeleteShader;
extern PFNGLDELETEPROGRAMPROC     glDeleteProgram;
extern PFNGLGETPROGRAMIVPROC      glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;

static bool canUseShaders()
{
   return glCreateShader != nullptr
      && glShaderSource != nullptr
      && glCompileShader != nullptr
      && glGetShaderiv != nullptr
      && glGetShaderInfoLog != nullptr
      && glCreateProgram != nullptr
      && glAttachShader != nullptr
      && glLinkProgram != nullptr
      && glDetachShader != nullptr
      && glDeleteShader != nullptr
      && glDeleteProgram != nullptr
      && glGetProgramiv != nullptr
      && glGetProgramInfoLog != nullptr;
}
static bool canUseFramebufferObjects()
{
	return glGenFramebuffers != nullptr
		&& glBindFramebuffer != nullptr
		&& glDeleteFramebuffers != nullptr
		&& glGenRenderbuffers != nullptr
		&& glDeleteRenderbuffers != nullptr
		&& glBindRenderbuffer != nullptr
		&& glRenderbufferStorage != nullptr
		&& glFramebufferRenderbuffer != nullptr;
}
#else
#include "OpenGL/gl.h"

static bool canUseShaders()
{
   return true;
}
static bool canUseFramebufferObjects()
{
   return true;
}
#endif

#include "OpenGLShaders.h"

#include "DrawGLUtils.h"

#include <log4cpp/Category.hh>

namespace
{
   bool shaderLinkSuceeded( GLuint programID )
   {
      GLint result = GL_FALSE;
      glGetProgramiv( programID, GL_LINK_STATUS, &result );
      if ( result == GL_FALSE )
      {
         int infoLogLength = 0;
         glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
         if ( infoLogLength > 0 )
         {
            std::vector<char> errorMessage( infoLogLength + 1 );
            char*             messagePtr = &errorMessage[0];
            glGetProgramInfoLog( programID, infoLogLength, NULL, messagePtr );
            static log4cpp::Category &logger_opengl = log4cpp::Category::getInstance( std::string( "log_opengl" ) );
            logger_opengl.error( "shader-link failure: '%s'", messagePtr );
         }
      }
      return result == GL_TRUE;
   }

   static bool shaderCompileSuceeded( GLuint shaderID )
   {
      GLint result = GL_FALSE;
      glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
      if ( result == GL_FALSE )
      {
         int infoLogLength = 0;
         glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );
         if ( infoLogLength > 0 )
         {
            std::vector<char> errorMessage( infoLogLength + 1 );
            char*             messagePtr = &errorMessage[0];
            glGetShaderInfoLog( shaderID, infoLogLength, NULL, messagePtr );
            static log4cpp::Category &logger_opengl = log4cpp::Category::getInstance( std::string( "log_opengl" ) );
            logger_opengl.error( "shader-compile failure: '%s'", messagePtr );
         }
      }
      return result == GL_TRUE;
   }
}

bool OpenGLShaders::HasShaderSupport()
{
   return canUseShaders();
}

bool OpenGLShaders::HasFramebufferObjects()
{
   return canUseFramebufferObjects();
}

unsigned OpenGLShaders::compile( const std::string& vertexSource, const std::string& fragmentSource )
{
    static log4cpp::Category& logger_opengl = log4cpp::Category::getInstance(std::string("log_opengl"));

    LOG_GL_ERRORV(GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER ));
   const GLchar* vertexShaders[] = { vertexSource.c_str() };
   LOG_GL_ERRORV(glShaderSource( vertexShader, 1, vertexShaders, NULL ));
   LOG_GL_ERRORV(glCompileShader( vertexShader ));
   if (!shaderCompileSuceeded(vertexShader))
   {
       LOG_GL_ERRORV(glDeleteShader(vertexShader));
       return 0;
   }

   LOG_GL_ERRORV(GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER ));
   const GLchar* fragmentShaders[] = { fragmentSource.c_str() };
   LOG_GL_ERRORV(glShaderSource( fragmentShader, 1, fragmentShaders, nullptr ));
   LOG_GL_ERRORV(glCompileShader( fragmentShader ));
   if (!shaderCompileSuceeded(fragmentShader))
   {
       LOG_GL_ERRORV(glDeleteShader(vertexShader));
       LOG_GL_ERRORV(glDeleteShader(fragmentShader));
       logger_opengl.error("%s", (const char*)fragmentSource.c_str());
       return 0;
   }

   LOG_GL_ERRORV(GLuint program = glCreateProgram());
   LOG_GL_ERRORV(glAttachShader( program, vertexShader ));
   LOG_GL_ERRORV(glAttachShader( program, fragmentShader ));
   LOG_GL_ERRORV(glLinkProgram( program ));
   bool linkSuccess = shaderLinkSuceeded( program );

   LOG_GL_ERRORV(glDetachShader( program, vertexShader ));
   LOG_GL_ERRORV(glDetachShader( program, fragmentShader ));

   LOG_GL_ERRORV(glDeleteShader( vertexShader ));
   LOG_GL_ERRORV(glDeleteShader( fragmentShader ));

   return linkSuccess ? program : 0;
}
