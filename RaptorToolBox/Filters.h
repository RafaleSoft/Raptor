// Filters.h: interface for the CFilters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERS_H__FA3B0FCF_C410_4469_B9B0_DEE07A8424A2__INCLUDED_)
#define AFX_FILTERS_H__FA3B0FCF_C410_4469_B9B0_DEE07A8424A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


RAPTOR_NAMESPACE


class RAPTOR_API CFilters
{
public:
    //! Install filters shaders into current instance of Raptor
    static bool glInstallFilters(void) ;

    //! Install one selected ilter shader into current instance of Raptor
	static bool glInstallFilter(const std::string& filterType);

private:
	CFilters();
	virtual ~CFilters();
};

#endif // !defined(AFX_FILTERS_H__FA3B0FCF_C410_4469_B9B0_DEE07A8424A2__INCLUDED_)
