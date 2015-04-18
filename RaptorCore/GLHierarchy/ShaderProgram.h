// ShaderProgram.h: interface for the CShaderProgram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)
#define AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_TEXTUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
    #include "GLHierarchy/TextureUnitSetup.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class RAPTOR_API CShaderProgram : public CPersistence  
{
public:
    //!  Fixed pipeline attributes binding shortcut that can help switching
    //! easily between the asm shaders and conventional pipelines which are exclusive.
    //! Nevertheless, GLSL bindings can coexist with conventionnal mapping.
    typedef enum GL_VERTEX_ATTRIB_t
	{
		POSITION,
		WEIGHTS,
		NORMAL,
		PRIMARY_COLOR,
		SECONDARY_COLOR,
		FOG_COORDINATE,
		ADDITIONAL_PARAM1,
		ADDITIONAL_PARAM2,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		TEXCOORD4,
		TEXCOORD5,
		TEXCOORD6,
		TEXCOORD7
	} GL_VERTEX_ATTRIB;


    //! The whole parameter set structure
	class RAPTOR_API CProgramParameters
	{
    public:
        //! This type defines the kind of parameter value defined here under
        typedef enum PARAMETER_KIND_t
        {
            VECTOR,
            MATRIX,
            ATTRIBUTE,
            SAMPLER
        } PARAMETER_KIND;

        //! This structure defines a parameter value
        typedef struct PROGRAM_PARAMETER_VALUE_t
        {
            //! The name of the value: it is matched to variables of the program ( @see GLSL )
            string				name;
            //! The kind of parameter : depending on the kind, only one of the four values here under is valid
            PARAMETER_KIND		kind;

            //! User values : a vector, a matrix, an attribute of a texture image unit.
			GL_COORD_VERTEX		vector;
			GL_MATRIX			matrix;
			GL_VERTEX_ATTRIB	attribute;
			CTextureUnitSetup::TEXTURE_IMAGE_UNIT	sampler;
        } PROGRAM_PARAMETER_VALUE;

    public:
        //! Default constructor. The parameter set is uninitialised, user
        //! must call the proper init method from the relevant shader.
		CProgramParameters():mValues() {};
		virtual ~CProgramParameters() {};

		//!	Remove all parameters
		void clear(void) { mValues.clear(); }

		//! Number of paremeters
		size_t getNbParameters(void) const { return mValues.size(); };

		//! Add a vertex as a parameter
		bool addParameter(const std::string& name, const GL_COORD_VERTEX& vertex);

		//! Add a matrix as a parameter
		bool addParameter(const std::string& name, const GL_MATRIX& matrix);

		//! Add an attribute as a parameter
		bool addParameter(const std::string& name, GL_VERTEX_ATTRIB attrib);

		//! Add a sampler as a parameter
		bool addParameter(const std::string& name, CTextureUnitSetup::TEXTURE_IMAGE_UNIT sampler);

        //! Access to vector parameters.
		PROGRAM_PARAMETER_VALUE& operator[](unsigned int v);

		//! Parameters assignment.
		CProgramParameters& operator=(const CProgramParameters& params);

    private:
        CProgramParameters(const CProgramParameters& ) {};

        //! There should not be a max value, it is inherited from  NV_vertex_program first drafts.
        vector<PROGRAM_PARAMETER_VALUE> mValues;
	};


public:
	virtual ~CShaderProgram();

    //! Program status: program may not be usable until valid state is true.
    //! isValid is equivalent to glGetProgramStatus but can be used without a gfx context.
    virtual bool isValid(void) const = 0;

	//!	Loads the assembler program describing OGL attributes
	//! processing program. The program must be correct,any other program
	//! loaded in a single objet is deleted and replaced by the one
	//! newly loaded.
	//! One must use several ShaderProgram objects to call several programs.
	//!	A default program is always valid however, it is the GL standard
	//! processing pipeline. It can be specified passing "opengl" 
	//! as the program argument ( the default value for any new object )
	virtual bool glLoadProgram(const std::string &program) = 0;

	//!	Same behavior as above, but program source is external.
	virtual bool glLoadProgramFromStream(CRaptorIO &stream);

	//!	The Render method has the typical bahaviour
	//!	of all Raptor Render methods : if the object
	//!	has no geometry info, Render sets GL state
	//!	for the next GL primitives rendered.
	//!	This behaviour is the same with vertex shaders
	virtual void glRender(void) = 0;

	//!	This method safely exists the vertex shader mode
	//!
	//!	TODO : backup program parameters and vertex state
	//!		to enable a safe stop/restart to switch among
	//!		various vertex programs
	virtual void glStop(void) = 0;

	//!	This method returns true if the program loaded fits
	//!	within the capabilities computed in the method below.
	//!	If result is true, it means the program will run properly
	//!	giving expected result.
	virtual bool glGetProgramStatus(void) = 0;

	//!	This method sets a program parameter ( 4 components vector ):
	//! The number of parameters is limited by hardware ( get shader caps in derived classes ). 
    //! The first parameter selects the program paramater, the second is the actual data.
    //! CAUTION : Accessing parameters directly is unsafe and should be avoided, except for performance issues.
	virtual void glProgramParameter( unsigned int numParam,
                                     const GL_COORD_VERTEX &v) = 0;

	//! Same as above but passes a color instead of a geo vector
	virtual void glProgramParameter( unsigned int numParam,
                                     const CColor::RGBA &v) = 0;

    //! This method can be used to pass in the whole parameter set.
    //! They will be actually applied after a successfull link is issued.
    //! This method is safe and can be used in engine parallel pipe.
    void setProgramParameters(const CProgramParameters &v);

	//! Inherited from CPersistence
    DECLARE_IO
	DECLARE_CLASS_ID(CShaderProgramClassID,"ShaderProgram",CPersistence)


protected:
	CShaderProgram(const CPersistence::CPersistenceClassID& id,const std::string& name="SHADER_PROGRAM");
	
    //! Sub classes must call this method to initialise shader attributes.
    virtual void	glInitShaders();


	RAPTOR_HANDLE			m_handle;
	bool					m_bApplyParameters;
	CProgramParameters	    m_parameters;

	static bool				m_bVertexReady;
	static bool				m_bFragmentReady;
    static bool				m_bVertexProgramReady;
	static bool				m_bFragmentProgramReady;
	static bool				m_bGeometryProgramReady;


private:
    //! Denied operators
	CShaderProgram();
    CShaderProgram(const CShaderProgram&);
    CShaderProgram& operator=(const CShaderProgram& ) { return *this;};
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_SHADERPROGRAM_H__936BEC73_3903_46CE_86C9_9CA0005B31F5__INCLUDED_)

