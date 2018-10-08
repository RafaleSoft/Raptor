// ShaderCompiler.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"

#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "System/RaptorErrorManager.h"
#include "System/RaptorApplication.h"
#include "System/RenderEntryPoint.h"
#include "System/RaptorConsole.h"

#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/RenderingProperties.h"
#include "GLHierarchy/Shader.h"
#include "GLHierarchy/VertexShader.h"
#include "GLHierarchy/VertexProgram.h"
#include "GLHierarchy/FragmentShader.h"
#include "GLHierarchy/FragmentProgram.h"

#include "ToolBox/Imaging.h"
#include "MFCExtension/RaptorMFCApplication.h"
#include "MFCExtension/CWnd/GLFrameWnd.h"

#include <strstream>

RAPTOR_NAMESPACE



class CDisplay : public CRenderEntryPoint
{
public:
    CDisplay(CFrameWnd *wnd,CRaptorDisplay *pDisplay)
    {
        nbShadersOK = nbShadersKO = 0;

        m_wnd = wnd;
        m_pDisplay = pDisplay;

		CTextureFactory &factory = CTextureFactory::getDefaultFactory();
        background = factory.glCreateTexture(	ITextureObject::CGL_COLOR24_ALPHA,
												CTextureObject::CGL_OPAQUE,
												ITextureObject::CGL_BILINEAR);
        background->glSetTransparency(255);
        factory.glLoadTexture(background,"Raptor_splash.jpg");
        //sfactory.glLoadTexture(T,"Dune.tif");
    };

    virtual ~CDisplay() 
    {
        fileSources.clear();
    };

    int getStatus(void)
    {
        if (nbShadersOK == 0)
            return -2;
        else if (nbShadersKO > 0)
            return -1;
        else
            return 0;
    }

    bool parse(const string& fname);

    void addSource(const char* src)
    { 
        if (src) 
            fileSources.push_back(src); 
    };

    virtual void glRender();

private:
    void checkVShader(const string &);
    void checkFShader(const string &);
    void checkVProgram(const string &);
    void checkFProgram(const string &);
    
    CFrameWnd *m_wnd;
    CRaptorDisplay *m_pDisplay;
    CTextureObject *background;

    unsigned int nbShadersOK;
    unsigned int nbShadersKO;

    vector<string>  fileSources;
};

void CDisplay::checkVShader(const string &src)
{
    string code = src;
     
    //  Remove comments
    string::size_type pos = code.find('#');
    while (pos != string::npos)
    {
        string::size_type pos2 = code.find('\n',pos);
        code.erase(pos,pos2-pos);
        pos = code.find('#',pos);
    }

    //  Remove special chars
    pos = code.find('\\');
    while (pos != string::npos)
    {
        code.erase(pos,2);
        pos = code.find('\\',pos);
    }

    CVertexShader vs;
    if (vs.glLoadProgram(code))
        nbShadersOK++;
    else
        nbShadersKO++;
}

void CDisplay::checkFShader(const string &src)
{
    string code = src;

    //  Remove comments
    string::size_type pos = code.find('#');
    while (pos != string::npos)
    {
        string::size_type pos2 = code.find('\n',pos);
        code.erase(pos,pos2-pos);
        pos = code.find('#',pos);
    }

    //  Remove special chars
    pos = code.find('\\');
    while (pos != string::npos)
    {
        code.erase(pos,2);
        pos = code.find('\\',pos);
    }

    CFragmentShader fs;
    if (fs.glLoadProgram(code))
        nbShadersOK++;
    else
        nbShadersKO++;
}

void CDisplay::checkVProgram(const string &src)
{
    string code = src;

    //  Remove special chars
    string::size_type pos = code.find('\\');
    while (pos != string::npos)
    {
        code.erase(pos,2);
        pos = code.find('\\',pos);
    }

    CVertexProgram vp;
    if (vp.glLoadProgram(code))
            nbShadersOK++;
    else
        nbShadersKO++;
}

void CDisplay::checkFProgram(const string &src)
{
	string code = src;

    //  Remove special chars
    string::size_type pos = code.find('\\');
    while (pos != string::npos)
    {
        code.erase(pos,2);
        pos = code.find('\\',pos);
    }

    CFragmentProgram fp;
    if (fp.glLoadProgram(code))
            nbShadersOK++;
    else
        nbShadersKO++;
}

bool CDisplay::parse(const string& fname)
{
    FILE *source = NULL;

    source = fopen(fname.data(),"r");
    if (source == NULL)
        return false;

    if (fseek(source,0,SEEK_END))
        return false;

    fpos_t pos = 0;
    if (fgetpos(source,&pos))
        return false;

    if (fseek(source,0,SEEK_SET))
    {
        fclose(source);
        return false;
    }

    char *code = new char[pos+1];
    memset(code,0,pos+1);
    size_t readSize = 0;
    if ((readSize=fread(code,1,pos,source)) > 0)
    {
        //! Is the file a whole vertex/fragment shader ?
        if (!strncmp(code,"!!ARBvp",7))
            checkVShader(string(code));
        else if (!strncmp(code,"!!ARBfp",7))
            checkFShader(string(code));
        //! Is the file a fragment program ?
        else if (0 != strstr(code,"gl_FragColor"))
            checkFProgram(string(code));
        //! Is the file a vertex program ?
        else if (0 != strstr(code,"gl_Position"))
            checkVProgram(string(code));
        //! Else when can only find shaders in a string in the file using prefixes
        else
        {
            char *strpos = code;

            while (strpos != NULL)
            {
                // search vs/fs in strings
                strpos = strchr(strpos,'"');

                // here I should string the string
                if (strpos != NULL)
                    strpos++;

                if ((strpos != NULL) && ((strpos - code) < (readSize - 7)) && (!strncmp(strpos,"!!ARBvp",7)))
                {
                    char *strpos2 = strchr(strpos+1,'"');
                    if (strpos2)
                        checkVShader(string(strpos,strpos2-strpos));
                    strpos = strpos2;
                }

                else if ((strpos != NULL) && 
						((strpos - code) < (readSize - 7)) && 
						(!strncmp(strpos,"!!ARBfp",7)))
                {
                    char *strpos2 = strchr(strpos+1,'"');
                    if (strpos2)
                        checkFShader(string(strpos,strpos2-strpos));
                    strpos = strpos2;
                }

                else if (strpos != NULL)
                {
                    char *strpos2 = strchr(strpos+1,'"');
                    if (strpos2)
                    {
                        string src(strpos,strpos2-strpos);
                        if (string::npos != src.find("gl_FragColor"))
                            checkFProgram(src);
                        else if (string::npos != src.find("gl_Position"))
                            checkVProgram(src);
                    }
                }
            }
        }
    }

    delete[] code;

    if (!feof(source))
    {
        fclose(source);
        return false;
    }

    return (!fclose(source));
}

void CDisplay::glRender(void)
{
    if (m_pDisplay == NULL)
        return ;

    CClientDC DC(m_wnd);
    RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS,DC.m_hDC);
	m_pDisplay->glvkBindDisplay(device);

    if (!fileSources.empty())
    {
        parse(fileSources.back());

        fileSources.pop_back();

        CRaptorConsole *pConsole = Raptor::GetConsole();
        pConsole->clear();

        CRaptorConsole::TEXT_ITEM item;
        item.offset = 20;
        strstream text;
        text << "Shaders compiled: " << nbShadersOK << ends;
        item.text = text.str();
        pConsole->addItem(item);

        item.offset = 20;
        strstream text2;
        text2 << "Shaders failed: " << nbShadersKO << ends;
        item.text = text2.str();
        pConsole->addItem(item);
    }

	background->glvkRender();

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);glVertex3f(-1.33f,-1.0f,-1.0f);
        glTexCoord2f(1.0f,0.0f);glVertex3f(1.33f,-1.0f,-1.0f);
        glTexCoord2f(1.0f,1.0f);glVertex3f(1.33f,1.0f,-1.0f);
        glTexCoord2f(0.0f,1.0f);glVertex3f(-1.33f,1.0f,-1.0f);
    glEnd();

    m_pDisplay->glRender();

	m_pDisplay->glvkUnBindDisplay();
}



int main(int argc, char* argv[])
{
    if (argc <2)
    {
        Raptor::GetMessages()->displayMessage("No shader file to try to compile... exiting");
        exit(-1);
    }


    CRaptorConfig config;
    config.m_bAutoDestroy = false;
    config.m_bRelocation = true;
    config.m_uiPolygons = 100;
    config.m_uiVertices = 400;
	config.m_logFile = "ShaderCompiler.log";

    Raptor::glInitRaptor(config);

    CImaging::installImagers();

	unsigned int v = Raptor::GetVersion();
	stringstream title;
    title << "Raptor Shader compiler tool ";
    title << ((v>>24)&0xFF) << "." << ((v>>16)&0xFF) << "." << ((v>>8)&0xFF);
    title << " Release";


	CRaptorDisplayConfig glcs;
	glcs.width = 320;
	glcs.height = 240;
	glcs.x = 0;
	glcs.y = 0;
	glcs.caption = title.str();
	glcs.acceleration = CRaptorDisplayConfig::HARDWARE;
	glcs.double_buffer = true;
	glcs.depth_buffer = true;
	glcs.display_mode = CGL_RGBA| CGL_DEPTH;
    glcs.refresh_rate.fps = 15; // enough, do not need full speed    

    CRaptorApplication  *app = new CRaptorMFCApplication();
    app->initApplication();
    CFrameWnd *wnd = ((CRaptorMFCApplication*)app)->createRootWindow(glcs); 

    CDisplay *d = NULL;
    CClientDC *pDC = new CClientDC(wnd);
    RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS,pDC->m_hDC);

    CRaptorDisplay *pDisplay = Raptor::glCreateDisplay(glcs);
	if (pDisplay->glvkBindDisplay(device))
	{
		if (!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME) ||
			!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_SHADER_EXTENSION_NAME) ||
			!Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_PROGRAM_EXTENSION_NAME) ||
			!Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_SHADER_EXTENSION_NAME) ||
			!Raptor::glIsExtensionSupported(GL_ARB_GEOMETRY_SHADER4_EXTENSION_NAME))
        {
            Raptor::GetMessages()->displayMessage("Sorry: Test cannot run : hardware OpenGL shaders not supported, exiting...");
            return -1;
        }

        d = new CDisplay(wnd,pDisplay);

        CRenderingProperties &props = pDisplay->getRenderingProperties();
        props.setTexturing(CRenderingProperties::ENABLE);
        props.setLighting(CRenderingProperties::DISABLE);

        CRaptorConsole *pConsole = Raptor::GetConsole();
        pConsole->glInit("",true);
        pConsole->showStatus(true);
		pConsole->showFPS(true);
		pConsole->showFrameTime(true);
        pConsole->activateConsole(true);

		pDisplay->glvkUnBindDisplay();
    }
    else
    {
        Raptor::GetMessages()->displayMessage("Sorry: Test cannot run : hardware OpenGL rendering not supported, exiting...");
        return -1;
    }

    delete pDC;


    for (int i = 1; i<argc;i++)
        d->addSource(argv[i]);

    app->run();

    int st = d->getStatus();

    delete d;
    Raptor::glQuitRaptor();

    return st;
}

#ifdef WIN32
#ifndef UNICODE
int WINAPI WinMain(HINSTANCE hInstance,  // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
					LPSTR lpCmdLine,      // pointer to command line
					int nCmdShow)          // show state of window
{
	int res = 0;
	if (AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		res = main(__argc, __argv);
#else
int AFXAPI AfxWinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance,
						_In_ LPTSTR lpCmdLine, int nCmdShow)
{
	int res = 0;
	wchar_t **wargv = __wargv;
	char **argv = new char*[__argc];
	for (int c = 0; c < __argc; c++)
		argv[c] = _strdup(CT2A(wargv[c]));
	if (AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		res = main(__argc, argv);
	for (int c = 0; c < __argc; c++)
		free(argv[c]);
	delete[] argv;
#endif
    AfxWinTerm();
    return res;
}
#endif

