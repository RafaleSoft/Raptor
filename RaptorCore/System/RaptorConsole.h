/***************************************************************************/
/*                                                                         */
/*  RaptorConsole.h                                                        */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
#define AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#ifndef __CGLTYPES_HPP__
    #include "CGLTypes.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CGLFont;
class CLexical;
class CMicroYacc;



class RAPTOR_API CRaptorConsole
{
public:
	//!	That structure stores data to write a console line on screen.
    typedef struct TEXT_ITEM_TAG
    {
		//!	horizontal position, in pixels.
        uint32_t	x_offset;
		//!	vertical position, in pixels (y axis in reversed, i.e. top left corner is 0, increasing value go to bottom left corner.
		uint32_t	y_offset;
		//!	text line to write.
        string		text;

		TEXT_ITEM_TAG() :x_offset(0), y_offset(0) {};
    } TEXT_ITEM;


	//!	This class abstracts an input collector for console user interaction.
	class RAPTOR_API CInputCollectorBase
	{
	public:
		CInputCollectorBase(void);
		virtual ~CInputCollectorBase(void);
		virtual void handleCharacterInput(char c) = 0;
		virtual void handleSpecialKeyInput(unsigned char c) = 0;
		virtual void handleMouseInput(int button, int xpos, int ypos) = 0;

		static void broadcastCharacterInput(char c);
		static void broadcastSpecialKeyInput(unsigned char c);
		static void broadcastMouseInput(int button, int xpos, int ypos);
	};

	//!	This class implements a generic user input collector.
	template <class T> class CInputCollector : public CInputCollectorBase
	{
	public:
		CInputCollector(T& holder)
			:CInputCollectorBase(),m_rHolder(holder) {};
		virtual ~CInputCollector(void) {};

		//! Forward keybord input to holder class
		void handleCharacterInput(char c)
		{ m_rHolder.handleCharacterInput(c); };

		//! Forward keybord input to holder class
		void handleSpecialKeyInput(unsigned char k)
		{ m_rHolder.handleSpecialKeyInput(k); };

		//! Forward mouse input to holder class
		void handleMouseInput(int button, int xpos, int ypos)
		{ m_rHolder.handleMouseInput(button,xpos,ypos); };

	private:
		CInputCollector() {};

		T& m_rHolder;
	};


public:
	//!	Constructor.
	CRaptorConsole();

	//!	Destructor.
	virtual ~CRaptorConsole();

    //! Generic rendering method to draw the console.
	void glRender(void);

    //! Initialise the console ( mainly font & parsers )
	//!	@param fontPath : the full path to the font file to use
	//! @param useVectors : if true, the vectorized form of the font is used.
	//!	Otherwise, the 2D bitmap font is used.
	void glInit(const std::string &fontPath = "",bool useVectors = false);

    //! This methods activates the console ( ready for input processing
    //!  and rendering ) if parameter is 'true', or deactivate it if parameter is false.
    //! It then returns the previous state of the console.
    bool activateConsole(bool activate);

    //! Returns activate state of the console
    bool isActive(void) const { return m_bIsActive; };

	//! Defines the char to 'auto'activate the console at keyboard input
	//! The default value is '²'
	//!	@return the previous char.
	char setAutoactivateChar(char autoChar)
	{ char old = autoActivateChar; autoActivateChar = autoChar; return old; };

    //! Runs a command file
	bool runBatch(const std::string& filename);

	//!	This method passes 'keyboard' input to the console.
	//! Application framework must handle input delay & repetitions.
	//!	@param c : a valid character input
	void handleInput(char c);

    //! Defines the console prompt. It can be used to display short status info.
    //! The default value is "Raptor>"
	void setPrompt(const std::string& prompt);

    //! Enables/disables the display of the status string.
    //! Returns the previous status
    bool showStatus(bool show);

    //! Enables/disables the display of the fps status ( status must be activated ).
    //! Returns the previous status ( by default, fps is activated )
    bool showFPS(bool show);

	//! Enables/disables the display of the inter-frame delay
    //! Returns the previous status ( by default, FrameTime is deactivated )
    bool showFrameTime(bool show);

    //! Enables/disables the display of the object count status 
    //! ( count is the numbre of rendered objects, status must be activated ).
    //! Returns the previous status ( by default, ObjectCount is deactivated )
    bool showObjectCount(bool show);

    //! Enables/disables the display of the triangle count status 
    //! ( count is the numbre of rendered triangles issued from objects, status must be activated ).
    //! Returns the previous status ( by default, TriangleCount is deactivated )
    bool showTriangleCount(bool show);

	//!	Returns the main font color.
	//! @return the main color.
	CColor::RGBA getColor(void) const { return m_color; };

    //! This method defines the console text color.
    //! The default is red, the background is transparent.
    void setColor(const CColor::RGBA& color);

	//! This method defines the console glow color.
    //! The default is black, the background is transparent.
    void setSecondColor(const CColor::RGBA& color);

    //! Thie method clears the console display:
    //! it removes output message and all text items.
    void clear(void);

    //! This method adds a text item
	//!	@param t : the item to add
	//! @return the new item id
    unsigned int addItem(TEXT_ITEM t);

	//! Updates an existing item.
	//!	@param t : an item containing new values
	//!	@param id : the id of the item to update
	//!	@return false if the id is not found.
	bool updateItem(TEXT_ITEM t,unsigned int id);


private:
	//! This method handles 'keyboard character' input : Application framework must
	//! manage keyboard and then pass the chars one by one using this method.
	//! Application specific chars must handled in the upper level, only valid text
	//! commands can be processed here.
	void handleCharacterInput(char c);

	//!	This method handles 'keyboard special key' input
	void handleSpecialKeyInput(unsigned char k);

	//! This method handles 'mouse' input : Application framework must
	//! manage mice and then pass the current status using this method.
	void handleMouseInput(int button, int xpos, int ypos);

	//!	Helper method to write a text item on screen
	void glWriteLine(const std::string& line,int x,int y);

    void displayHelp(void);


    bool	m_bIsActive;
    bool	m_bShowStatus;
    bool	m_bShowFPS;
	bool	m_bShowFrameTime;
    bool	m_bShowObjects;
	bool	m_bShowTriangles;
	bool	m_bUseVectors;
    CColor::RGBA   m_color;
	CColor::RGBA   m_secondColor;

	CInputCollector<CRaptorConsole>	*m_pInput;
	CGLFont			*m_pFont;

    string  promptString;
	string	cmdString;
	string	lastResult;
	char	autoActivateChar;

    vector<TEXT_ITEM>   m_items;

	CLexical	*lex;
	CMicroYacc	*yacc;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)

