#include "GLBuilder.h"
#include <GL/gl.h>
#include <GL/glx.h>
#include <iostream>


#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "System/Raptor.h"
#endif


int glCreateContext()
{
    Display* pGlobalDisplay = XOpenDisplay(NULL);
    int dummy = 0;
    if ((NULL == pGlobalDisplay) || !glXQueryExtension(pGlobalDisplay, &dummy, &dummy))
    {
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_FATAL,
                                                        "OpenGL GLX Extensions are not supported or display is invalid!");
        return -1;
    }

    GLXContext	    glhrc = NULL;

    int flags[256];
    int numFlags = 0;
    const int	colorbits = 8;
    const int	alphabits = 8;
	  const int	depthbits = 24;
	  const int	stencilbits = 8;

    flags[numFlags++] = GLX_RGBA;
    flags[numFlags++] = GLX_USE_GL;
    flags[numFlags++] = GLX_RED_SIZE;
    flags[numFlags++] = colorbits;
    flags[numFlags++] = GLX_GREEN_SIZE;
    flags[numFlags++] = colorbits;
    flags[numFlags++] = GLX_BLUE_SIZE;
    flags[numFlags++] = colorbits;
    
    flags[numFlags++] = GLX_ALPHA_SIZE;
    flags[numFlags++] = alphabits;
    flags[numFlags++] = GLX_DOUBLEBUFFER;
    flags[numFlags++] = GLX_DEPTH_SIZE;
    flags[numFlags++] = depthbits;
    flags[numFlags++] = GLX_STENCIL_SIZE;
    flags[numFlags++] = alphabits;

    flags[numFlags++] = None;

    int screen = DefaultScreen(pGlobalDisplay);
    Display *display = (Display*)(device.handle);
    XVisualInfo *visual = glXChooseVisual(display, screen, flags);

    if ( visual == 0 ) 
    {
        Raptor::GetErrorManager()->generateRaptorError( Global::COpenGLClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_FATAL,
                                                        "Raptor Context Manager failed to choose pixel format");
        return -1; 
    }

    glhrc = glXCreateContext(display,visual,0,True);
    if (!glhrc)
    {
        Raptor::GetErrorManager()->generateRaptorError( Global::COpenGLClassID::GetClassId(),
                                                        CRaptorErrorManager::RAPTOR_FATAL,
                                                        "Raptor Context Manager failed to create OpenGL context");
        return -1;
    }

    glXMakeCurrent(pGlobalDisplay, glhrc);
    glInitExtensions();
    glXMakeCurrent(pGlobalDisplay, NULL);

    return 0;
}

int main(int argc, char *argv[])
{
  CGLBuilder builder;

  if (!glCreateContext())
  {
    std::cout << "Failed to query OpenGL context. " << std::endl;
    return -1;
  }

  const vector<CGLBuilder::EXTENSION>& extensions = builder.getExtensions();
  for (unsigned int i=0;i<extensions.size();i++)
	{
		const CGLBuilder::EXTENSION& extension = extensions[i];
    std::cout << "extension: " << extension.extensionName;
    std::cout << ((extension.active)?" O ":" x ") << std::endl;
  }

  if (!builder.checkConsistency(false))
  {
    std::cout << "Inconsistency found in extension profile. Header file not generated." << std::endl;
    return -1;
  }

  return 0;
}
