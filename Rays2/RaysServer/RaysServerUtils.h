// ServerTransport.h: interface for the CServerTransport class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
#define AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace RaysServer {

	public ref class RaysServerUtils
	{
	public:
		//!	Convert a System::String into a char*.
		static char* convertSystemString(System::String^ str);

	private:
		RaysServerUtils() {};
		~RaysServerUtils() {};
	};
}

#endif // !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
