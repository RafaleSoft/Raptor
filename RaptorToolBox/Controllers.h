// Controllers.h: interface for the CController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLLERS_H__CC9BBAE1_0481_4CC0_A421_7BB690656844__INCLUDED_)
#define AFX_CONTROLLERS_H__CC9BBAE1_0481_4CC0_A421_7BB690656844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/IViewPoint.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "System/RaptorConsole.h"
#endif

RAPTOR_NAMESPACE

class RAPTOR_API CControllers
{
public:
	class RAPTOR_API CController
	{
	public:
		//! @see RaptorConsole
		virtual void handleCharacterInput(char c) = 0;

		//! @see RaptorConsole
		virtual void handleSpecialKeyInput(unsigned char k) = 0;

		//! @see RaptorConsole
		virtual void handleMouseInput(int button, int xpos, int ypos) = 0;

	public:
		//! Reverse axis for mouse input
		//!	@return the previous reverse state
		bool reverseXAxis(void);
		bool reverseYAxis(void);
		bool reverseZAxis(void);

		//!	Scales controller interactions
		void scale(float sx,float sy,float sz);


	protected:
		CController();
		virtual ~CController();

		//! Reverse axis status.
		bool	m_bReverseXAxis;
		bool	m_bReverseYAxis;
		bool	m_bReverseZAxis;
		float	m_fScaleX;
		float	m_fScaleY;
		float	m_fScaleZ;


	private:
		//!	The input source
		CRaptorConsole::CInputCollector<CController>	*m_pInput;
	};

	static CController *createViewpointController(IViewPoint* pVp);

private:
	CControllers(void);
	~CControllers(void);
};

#endif //!defined(AFX_CONTROLLERS_H__CC9BBAE1_0481_4CC0_A421_7BB690656844__INCLUDED_)

