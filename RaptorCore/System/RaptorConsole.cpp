/***************************************************************************/
/*                                                                         */
/*  RaptorConsole.cpp                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "RaptorConsole.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
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
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
  #include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif

#include "Subsys/Interpreters.h"



static const float CONSOLE_DELAY = 0.016667f;

RAPTOR_NAMESPACE

CRaptorConsole::CInputCollectorBase::CInputCollectorBase(void)	
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	instance.inputCollectors.push_back(this);
}

CRaptorConsole::CInputCollectorBase::~CInputCollectorBase(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = instance.inputCollectors.begin();
	while (it != instance.inputCollectors.end())
	{
		if (*it == this)
		{
			instance.inputCollectors.erase(it);
			break;
		}
		it++;
	}
}

void CRaptorConsole::CInputCollectorBase::broadcastCharacterInput(char c)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = instance.inputCollectors.begin();
	while (it != instance.inputCollectors.end())
		(*it++)->handleCharacterInput(c);
}

void CRaptorConsole::CInputCollectorBase::broadcastSpecialKeyInput(unsigned char c)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = instance.inputCollectors.begin();
	while (it != instance.inputCollectors.end())
		(*it++)->handleSpecialKeyInput(c);
}

void CRaptorConsole::CInputCollectorBase::broadcastMouseInput(int button, int xpos, int ypos)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	vector<CRaptorConsole::CInputCollectorBase*>::iterator it = instance.inputCollectors.begin();
	while (it != instance.inputCollectors.end())
		(*it++)->handleMouseInput(button,xpos,ypos);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CRaptorConsole::CRaptorConsole():
    m_pFont(NULL), m_pInput(NULL),
	m_bIsActive(false),m_bShowStatus(false),
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

unsigned int CRaptorConsole::addItem(CGLFont::FONT_TEXT_ITEM &t)
{
	unsigned int id = m_items.size();
    m_items.push_back(t);
	return id;
}

bool CRaptorConsole::updateItem(CGLFont::FONT_TEXT_ITEM &t,unsigned int id)
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
        RAPTOR_NO_ERROR(COpenGL::COpenGLClassID::GetClassId(),msg);
    }
    else
    {
        msg = "Raptor console failed to load batch " + filename;
		RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), msg);
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
				RAPTOR_NO_ERROR(COpenGL::COpenGLClassID::GetClassId(), msg);
            }
            else
            {
                msg += " script error: " + yacc->getErrorMsg();
				RAPTOR_ERROR(COpenGL::COpenGLClassID::GetClassId(), msg);
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
	CGLFont::FONT_TEXT_ITEM t;

	m_items.clear();

	t.x_offset = 1; t.text = " ";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "help: display this help";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "clear: clear the console";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "select \"<ObjectName>\": selects an object as current object for subsequent commands";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "scale(float sx,float sy,float sz): scales the selected object in all dimensions";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "translate(float tx,float ty,float tz): translates the selected object in all dimensions";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "translate_absolute(float tx,float ty,float tz): moves the selected object to the specified position";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "visible(<true|false>): turns visible/invisible the selected object";
	m_items.push_back(t);
	t.x_offset = 1; t.text = "rotation(float a,float ax,float ay,float az): rotates the selected object of the specified angle(a) around the axis (ax,ay,az)";
	m_items.push_back(t);
}

bool CRaptorConsole::glInit(const std::string &fontPath,bool useVectors)
{
	if (NULL != m_pInput)
		delete m_pInput;
	m_pInput = new CInputCollector<CRaptorConsole>(*this);
    m_bIsActive = false;
	m_bUseVectors = useVectors;

	string filepath = fontPath;
	if ((filepath.empty()) && (!useVectors))
	{
		CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
		filepath = dataManager->exportFile("lucon.ttf");
	}

	if ((!filepath.empty()) && (!useVectors))
    {
		m_pFont = CGLFontFactory::glCreateTextureFont(filepath,8,true,"console_font");
		if ((NULL != m_pFont) && (!m_pFont->glGenGlyphs(1.0f,0.0f,1.0f)))
		{
			delete m_pFont;
			m_pFont = NULL;
			return false;
		}
	}
	else
	{
		m_pFont = CGLFontFactory::createVectorFont(filepath,8,"console_font");
		if ((NULL != m_pFont) && (!m_pFont->glGenGlyphs(1.0f,0.0f,0.35f)))
		{
			delete m_pFont;
			m_pFont = NULL;
			return false;
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

	return true;
}

void CRaptorConsole::glRender(void)
{
    if ((m_pFont != NULL) && m_bIsActive)
    {
	    glPushAttrib(GL_ENABLE_BIT);
	    
		glDisable(GL_TEXTURE_2D);
	    glDisable(GL_LIGHTING);
	    glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

		int currentLine = -1;
		std::vector<CGLFont::FONT_TEXT_ITEM> lines;

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
				lines.push_back(CGLFont::FONT_TEXT_ITEM(1, currentLine+=10, status.str(), m_color));
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
				lines.push_back(CGLFont::FONT_TEXT_ITEM(1, currentLine+=10, status.str(), m_color));
            }
            
			CRaptorInstance &instance = CRaptorInstance::GetInstance();
            if (m_bShowObjects)
            {
                unsigned int nbRendered = instance.iRenderedObjects;
                stringstream status2;
                status2 << "nb objs: " << nbRendered;
				lines.push_back(CGLFont::FONT_TEXT_ITEM(1, currentLine+=10, status2.str(), m_color));
            }

            if (m_bShowTriangles)
            {
                unsigned int nbRendered = instance.iRenderedTriangles;
                stringstream status3;
                status3 << "nb tris: " << nbRendered;
				lines.push_back(CGLFont::FONT_TEXT_ITEM(1, currentLine+=10, status3.str(), m_color));
            }
        }

		lines.push_back(CGLFont::FONT_TEXT_ITEM(1, currentLine+=10, promptString + cmdString, m_color));
		lines.push_back(CGLFont::FONT_TEXT_ITEM(1, currentLine+=10, lastResult, m_color));

        for (unsigned int i=0;i<m_items.size();i++)
        {
			CGLFont::FONT_TEXT_ITEM &t = m_items[i];
			lines.push_back(CGLFont::FONT_TEXT_ITEM(t.x_offset, currentLine+=10, t.text, m_color));
        }

		m_pFont->glWrite(lines);

	    glPopAttrib();
    }

	CATCH_GL_ERROR
}

