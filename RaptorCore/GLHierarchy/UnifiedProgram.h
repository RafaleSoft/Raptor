// UnifiedProgram.h: interface for the CUnifiedProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)
#define AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
    #include "GLHierarchy/ShaderProgram.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CUnifiedProgram : public CShaderProgram  
{
public:
	virtual ~CUnifiedProgram();

    //! Implements base class status
    virtual bool isValid(void) const { return m_bValid; };

    //! Loads a unique parameter immediately
    virtual void glProgramParameter(unsigned int numParam,
									const GL_COORD_VERTEX &v);

	//! Same as above but passes a color instead of a geo vector
	virtual void glProgramParameter(unsigned int numParam,
									const CColor::RGBA &v);


protected:
    CUnifiedProgram(const CPersistence::CPersistenceClassID &classId, 
		const std::string& name);

    //! determine parameter compatibility : compare GLSL to Raptor kind 
    //! @return true if parameter can be assigned ( though they need conversion )
    bool matchKind(unsigned int shaderKind, CProgramParameters::PARAMETER_KIND parameterKind);

    //! Updates locations of uniform variables
    void glQueryUniformLocations(RAPTOR_HANDLE program);

    //! Updates locations of attributes variables
    void glQueryAttributeLocations(RAPTOR_HANDLE program);

    //! size of precomputed locations
    int m_iMaxLocation;

    //! Parameter locations can be retrieven once per link and reused.
    typedef struct location_t
    {
        int				locationIndex;
        unsigned int	locationType;
    } location;
    vector<location> m_locations;

    //! Program has been re-linked, perform post processing again
    bool    m_bReLinked;

    //! Valid status
    bool    m_bValid;


private:
    //! factorize uniform settings
    void glParameter( unsigned int numParam,const float *v);

};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_UNIFIEDPROGRAM_H__CBCD5C66_88D0_4EAD_A5FD_B0F235B8FED6__INCLUDED_)

