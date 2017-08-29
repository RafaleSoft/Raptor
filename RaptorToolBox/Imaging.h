// Imaging.h: interface for the CImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
#define AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE


class RAPTOR_API CImaging  
{
public:
    //! Install image manipulators into current instance of Raptor
    static bool installImagers(void) ;

    //! Install one selected image manipulator into current instance of Raptor
	static bool installImager(const std::string& imageType);

private:
    CImaging();
	virtual ~CImaging();
};

#endif // !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
