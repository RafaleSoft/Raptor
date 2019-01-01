// Streaming.h: interface for the CStreaming class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMING_H__F765F8BE_C681_4B49_8535_B4B8CD341886__INCLUDED_)
#define AFX_STREAMING_H__F765F8BE_C681_4B49_8535_B4B8CD341886__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RAPTOR_API CStreaming  
{
public:
    //! Install video manipulators into current instance of Raptor
    static bool installStreamers(void);

    //!  Install video manipulators into current instance of Raptor
	static bool installStreamer(const std::string& streamType);

private:
	CStreaming();
	~CStreaming();

};

#endif // !defined(AFX_STREAMING_H__F765F8BE_C681_4B49_8535_B4B8CD341886__INCLUDED_)
