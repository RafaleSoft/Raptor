// RaptorConsole.cpp: implementation of the CRaptorConsole class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "RaptorConsole.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_GLFONT_H__D451FE62_5FE1_11D3_9142_BA23BC92E77C__INCLUDED_)
	#include "GLHierarchy/GLFont.h"
#endif
#if !defined(AFX_GLFONTFACTORY_H__E18CD490_4CB0_4ECA_916D_85B155FF04C3__INCLUDED_)
	#include "GLHierarchy/GLFontFactory.h"
#endif
#if !defined(AFX_GL2DTEXTUREFONT_H__7122B2F2_8D47_492F_8738_71FE06D8BA21__INCLUDED_)
	#include "GLHierarchy/GL2DTextureFont.h"
#endif
#if !defined(AFX_GLVECTORFONT_H__C21ADBEA_705D_43EA_A45F_F016233F7507__INCLUDED_)
	#include "GLHierarchy/GLVectorFont.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
  #include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

#include "Subsys/Interpreters.h"

static const float CONSOLE_DELAY = 0.016667f;

RAPTOR_NAMESPACE

CRaptorConsole::CInputCollectorBase::CInputCollectorBase(void)	
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	status.inputCollectors.push_back(this);
}

CRaptorConsole::CInputCollectorBase::~CInputCollectorBase(void)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = status.inputCollectors.begin();
	while (it != status.inputCollectors.end())
	{
		if (*it == this)
		{
			status.inputCollectors.erase(it);
			break;
		}
		it++;
	}
}

void CRaptorConsole::CInputCollectorBase::broadcastCharacterInput(char c)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = status.inputCollectors.begin();
	while (it != status.inputCollectors.end())
		(*it++)->handleCharacterInput(c);
}

void CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(unsigned char c)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = status.inputCollectors.begin();
	while (it != status.inputCollectors.end())
		(*it++)->handleSpecialKeyInput(c);
}

void CRaptorConsole::CInputCollectorBase::broadcastMouseInput(int button, int xpos, int ypos)
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = status.inputCollectors.begin();
	while (it != status.inputCollectors.end())
		(*it++)->handleMouseInput(button,xpos,ypos);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CRaptorConsole::CRaptorConsole():
    m_pFont(NULL),m_bIsActive(false),m_bShowStatus(false),
    m_bShowFPS(true),m_bShowObjects(false),m_bShowTriangles(false),
	m_bShowFrameTime(false),m_bUseVectors(false)
{
    promptString = "Raptor>";
	cmdString = "";
	lastResult = "OK";
	autoActivateChar = '²';

	lex = NULL;
	yacc = NULL;

    m_color.r = 1.0f;
    m_color.g = 0.3f;
    m_color.b = 0.2f;
    m_color.a = 1.0f;

	m_secondColor.r = 0.1f;
    m_secondColor.g = 0.1f;
    m_secondColor.b = 0.1f;
    m_secondColor.a = 1.0f;
}

CRaptorConsole::~CRaptorConsole()
{
    if (yacc != NULL)
        delete yacc;
    if (lex != NULL)
        delete lex;
    if (m_pFont != NULL)
        delete m_pFont;
	if (m_pInput != NULL)
		delete m_pInput;
}

void CRaptorConsole::setColor(const CColor::RGBA& color)
{
    m_color = color;
}

void CRaptorConsole::setSecondColor(const CColor::RGBA& color)
{
    m_secondColor = color;
}

bool CRaptorConsole::showStatus(bool show)
{
    bool ret = m_bShowStatus;
    m_bShowStatus = show;

    return ret;
}

bool CRaptorConsole::showFPS(bool show)
{
    bool ret = m_bShowFPS;
    m_bShowFPS = show;

    return ret;
}

bool CRaptorConsole::showFrameTime(bool show)
{
	bool ret = m_bShowFrameTime;
    m_bShowFrameTime = show;

    return ret;
}

bool CRaptorConsole::showObjectCount(bool show)
{
    bool ret = m_bShowObjects;
    m_bShowObjects = show;

    return ret;
}

bool CRaptorConsole::showTriangleCount(bool show)
{
    bool ret = m_bShowTriangles;
    m_bShowTriangles = show;

    return ret;
}

void CRaptorConsole::setPrompt(const std::string& prompt)
{
    promptString = prompt;
}

bool CRaptorConsole::activateConsole(bool activate)
{
    bool ret = m_bIsActive;
    m_bIsActive = activate;

    return ret;
}

unsigned int CRaptorConsole::addItem(TEXT_ITEM t)
{
	unsigned int id = m_items.size();
    m_items.push_back(t);
	return id;
}

bool CRaptorConsole::updateItem(TEXT_ITEM t,unsigned int id)
{
	if (id >= m_items.size())
		return false;

	m_items[id] = t;
	return true;
}

void CRaptorConsole::clear(void)
{
    m_items.clear();

	cmdString = "";
	lastResult = "OK";
}

bool CRaptorConsole::runBatch(const std::string& filename)
{
    if (!m_bIsActive) 
        return false;

    if ((lex == NULL) || (yacc == NULL))
        return false;

    CRaptorIO* IO = CRaptorIO::Create(filename.data(),CRaptorIO::DISK_READ);
    bool ret = ((IO != NULL) && (IO->getStatus() == CRaptorIO::IO_OK));

    CRaptorErrorManager *const errMgr = Raptor::GetErrorManager();
    string msg;
    if (ret)
    {
        msg = "Raptor console running batch " + filename;
        RAPTOR_NO_ERROR(Global::COpenGLClassID::GetClassId(),msg);
                                                         
    }
    else
    {
        msg = "Raptor console failed to load batch " + filename;
        RAPTOR_ERROR(Global::COpenGLClassID::GetClassId(),msg);
    }

    while (ret)
    {
        string command;
        *IO >> command;
        ret = (IO->getStatus() == CRaptorIO::IO_OK);

        if (ret && command.size() > 0)
        {
            lex->InitLex(command.data());
		    ret = ret && (yacc->Analyse() > 0);
		    lex->FinishLex();

            msg = "    batch command [";
            msg += command;
            msg += "]";
            if (ret)
            {
                msg += " OK: " + yacc->getErrorMsg();
                RAPTOR_NO_ERROR(Global::COpenGLClassID::GetClassId(),msg);
            }
            else
            {
                msg += " script error: " + yacc->getErrorMsg();
				RAPTOR_ERROR(Global::COpenGLClassID::GetClassId(),msg);
            }
        }
    }

    delete IO;
    return ret;
}

void CRaptorConsole::handleInput(char c)
{
	if (c == autoActivateChar)
	{
		m_bIsActive = !m_bIsActive;
		return;
	}
	else if (!m_bIsActive)
        return;

	switch (c)
	{
		case 8:
			if (cmdString.size() > 0)
				cmdString.erase(cmdString.size()-1,1);
			break;
		case 13:
		{
			if (lex != NULL)
			{
				lex->InitLex(cmdString.data());
				if (yacc->Analyse() < 0)
					lastResult = "script error. " + yacc->getErrorMsg();
				else
                {
					lastResult = "OK: " + yacc->getErrorMsg();

                    if (yacc->DisplayHelp())
                    {
                        lastResult = "<Raptor Console help >";
                        displayHelp();
                    }
					else if (yacc->Clear())
					{
						clear();
					}
                }

				lex->FinishLex();
			}
			cmdString = "";
			break;
		}
		default:
			cmdString = cmdString + c;
			break;
	}
}

void CRaptorConsole::handleCharacterInput(char c)
{
}

void CRaptorConsole::handleSpecialKeyInput(unsigned char k)
{
}

void CRaptorConsole::handleMouseInput(int button, int xpos, int ypos)
{
}

void CRaptorConsole::displayHelp(void)
{
	TEXT_ITEM t;

	m_items.clear();

	t.offset = 1; t.text = " ";
	m_items.push_back(t);
	t.offset = 1; t.text = "help: display this help";
	m_items.push_back(t);
	t.offset = 1; t.text = "clear: clear the console";
	m_items.push_back(t);
	t.offset = 1; t.text = "select \"<ObjectName>\": selects an object as current object for subsequent commands";
	m_items.push_back(t);
	t.offset = 1; t.text = "scale(float sx,float sy,float sz): scales the selected object in all dimensions";
	m_items.push_back(t);
	t.offset = 1; t.text = "translate(float tx,float ty,float tz): translates the selected object in all dimensions";
	m_items.push_back(t);
	t.offset = 1; t.text = "translate_absolute(float tx,float ty,float tz): moves the selected object to the specified position";
	m_items.push_back(t);
	t.offset = 1; t.text = "visible(<true|false>): turns visible/invisible the selected object";
	m_items.push_back(t);
	t.offset = 1; t.text = "rotation(float a,float ax,float ay,float az): rotates the selected object of the specified angle(a) around the axis (ax,ay,az)";
	m_items.push_back(t);
}

void CRaptorConsole::glInit(const std::string &fontPath,bool useVectors)
{
	m_pInput = new CInputCollector<CRaptorConsole>(*this);
    m_bIsActive = false;
	m_bUseVectors = useVectors;

	string filepath = fontPath;
	if ((filepath.empty()) && (!useVectors))
	{
		CRaptorDataManager  *dataManager = CRaptorDataManager::getInstance();
		filepath = dataManager->ExportFile("lucon.ttf");
	}

	if ((!filepath.empty()) && (!useVectors))
    {
		m_pFont = CGLFontFactory::glCreateTextureFont(filepath,8,true,"console_font");
		if ((NULL != m_pFont) && (!m_pFont->glGenGlyphs(1.0f,0.0f,1.0f)))
		{
			delete m_pFont;
			m_pFont = NULL;
		}
	}
	else
	{
		m_pFont = CGLFontFactory::createVectorFont(filepath,8,"console_font");
		if ((NULL != m_pFont) && (!m_pFont->glGenGlyphs(1.0f,0.0f,0.35f)))
		{
			delete m_pFont;
			m_pFont = NULL;
		}
        else
            m_bUseVectors = true;
	}

	if (lex == NULL)
	{
		lex = new CLexical;
		lex->DisplayError(false);
	}

	if (yacc == NULL)
	{
		yacc = new CMicroYacc(lex);
		yacc->DisplayError(false);
		yacc->InitAnalyse();
	}
}

void CRaptorConsole::glWriteLine(const std::string& line,int x,int y)
{
	if (!m_bUseVectors)
		m_pFont->glWrite(line,x,y);
	else
	{
		m_pFont->selectCurrentGlyphset(0);
		glPushMatrix();
		glColor4fv(m_secondColor);
		glLineWidth(3.0f);
		m_pFont->glWrite(line,x,-y);
		glColor4fv(m_color);
		glLineWidth(1.1f);
		m_pFont->glWrite(line,x,-y);
		glPopMatrix();
	}
}

void CRaptorConsole::glRender(void)
{
    if ((m_pFont != NULL) && m_bIsActive)
    {
        const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	    PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	    glPushAttrib(GL_ENABLE_BIT);
        if (glActiveTextureARB != NULL)
        {
            glActiveTextureARB(GL_TEXTURE3_ARB);
	        glDisable(GL_TEXTURE_2D);
            glActiveTextureARB(GL_TEXTURE2_ARB);
	        glDisable(GL_TEXTURE_2D);
            glActiveTextureARB(GL_TEXTURE1_ARB);
	        glDisable(GL_TEXTURE_2D);
            glActiveTextureARB(GL_TEXTURE0_ARB);
        }
	    glDisable(GL_TEXTURE_2D);
	    glDisable(GL_LIGHTING);
	    glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

		if (m_bUseVectors)
		{
			GLfloat viewport[4];
			glGetFloatv(GL_VIEWPORT,viewport);
			glMatrixMode(GL_PROJECTION);	
			glPushMatrix();
			glLoadIdentity();
			glOrtho(viewport[0],viewport[2],viewport[1],viewport[3],1,20);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(0.0f,viewport[3],-5.0f);
			glEnable(GL_BLEND);
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH,GL_NICEST);
		}

		CColor::RGBA oldColor;
		float lineWidth = 1.0f;
		glGetFloatv(GL_CURRENT_COLOR,oldColor);
		glGetFloatv(GL_LINE_WIDTH,&lineWidth);
	    glColor4fv(m_color);

		int currentLine = -1;

		if (m_bShowStatus)
        {
			CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();
            if ((m_bShowFPS) && (pDisplay != NULL))
            {
                stringstream status;
				status.precision(4);
                status << "fps: ";
				status << pDisplay->getFPS();
				status << " [";
				status << pDisplay->getRTFPS();
				status << "]";
                glWriteLine(status.str(),1,currentLine+=10);
            }

			if ((m_bShowFrameTime) && (pDisplay != NULL))
            {
                stringstream status;
				status.precision(4);
                status << "frame time: [";
				status << std::fixed;
				status << 1000.0f*pDisplay->getFrameTime();
				status << " ms] render time: [";
				status << 1000.0f*pDisplay->getRenderTime();
				status << " ms]";
                glWriteLine(status.str(),1,currentLine+=10);
            }
            
            if (m_bShowObjects)
            {
                unsigned int nbRendered = Global::GetInstance().getCurrentStatus().iRenderedObjects;
                stringstream status2;
                status2 << "nb objs: " << nbRendered;
                glWriteLine(status2.str(),1,currentLine+=10);
            }

            if (m_bShowTriangles)
            {
                unsigned int nbRendered = Global::GetInstance().getCurrentStatus().iRenderedTriangles;
                stringstream status3;
                status3 << "nb tris: " << nbRendered;
                glWriteLine(status3.str(),1,currentLine+=10);
            }
        }

		glWriteLine(promptString+cmdString,1,currentLine+=10);
	    glWriteLine(lastResult,1,currentLine+=10);


        for (unsigned int i=0;i<m_items.size();i++)
        {
            TEXT_ITEM &t = m_items[i];
            glWriteLine(t.text,t.offset,currentLine+=10);
        }

		if (m_bUseVectors)
		{
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

		glColor4fv(oldColor);
		glLineWidth(lineWidth);
	    glPopAttrib();
    }

	CATCH_GL_ERROR
}

