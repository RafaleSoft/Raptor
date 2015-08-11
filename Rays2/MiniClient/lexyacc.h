//////////////////////////////////////////////////////////////////////////
//	This file is generated MicroLex application from Rafale Soft Inc. (c)
//
//	Do not modify this file, your changes would be lost on next generation
//	This file defines and implements a 'lex like' parser as well as a
//	'yacc like' interpreter to use with your application that requires 
//	a small language to format your data files in an expandable way
///////////////////////////////////////////////////////////////////////////


#if !defined(__LEX_H__)
#define __LEX_H__

#define ATOM_integer_ID		1
#define ATOM_float_ID		2
#define ATOM_string_ID		3
#define ATOM_in_ID		4
#define ATOM_out_ID		5
#define ATOM_sep_ID		6
#define ATOM_set_ID		7
#define ATOM_width_ID		8
#define ATOM_height_ID		9
#define ATOM_variance_ID		10
#define ATOM_deflection_ID		11
#define ATOM_defraction_ID		12
#define ATOM_crease_ID		13
#define ATOM_separate_specular_ID		14
#define ATOM_none_ID		15
#define ATOM_vertex_ID		16
#define ATOM_shading_ID		17
#define ATOM_rendering_ID		18
#define ATOM_light_ID		19
#define ATOM_texture_ID		20
#define ATOM_triangle_ID		21
#define ATOM_geometry_ID		22
#define ATOM_mesh_ID		23
#define ATOM_import_ID		24
#define RULE_coord_ID		25
#define RULE_vertex_ID		26
#define RULE_triangle_ID		27
#define RULE_shadingoption_ID		28
#define RULE_shadingoptions_ID		29
#define RULE_shading_ID		30
#define RULE_light_ID		31
#define RULE_texture_ID		32
#define RULE_renderoption_ID		33
#define RULE_renderoptions_ID		34
#define RULE_render_ID		35
#define RULE_parameter_ID		36
#define RULE_element_ID		37
#define RULE_elementname_ID		38
#define RULE_define_ID		39
#define RULE_config_ID		40
#define RULE_triangles_ID		41
#define RULE_vertices_ID		42
#define RULE_objectbase_ID		43
#define RULE_composition_ID		44
#define RULE_sphere_ID		45
#define RULE_extern_ID		46
#define RULE_object_ID		47
#define RULE_objects_ID		48
#define RULE_main_ID		49

#define BUFFER_SIZE 256

//separators
#define NEW_LINE		10
#define	CARRIAGE_RETURN	13
#define	TAB				9
#define SPACE			32

#define IS_SEPARATOR(c) \
	((##c==NEW_LINE)||(##c==CARRIAGE_RETURN)||(##c==TAB)||(##c==SPACE))




#include <afxtempl.h>


//Syntaxic structures typedefs
typedef struct coord_tag
{
	float	st_float1;
	float	st_float3;
	float	st_float5;
	float	st_float7;
	int		m_step;

	coord_tag& operator=(const coord_tag &rsh);
}	coord_t;
typedef coord_t	*lpcoord_t;

typedef struct vertex_tag
{
	coord_t	st_coord1;
	coord_t	st_coord3;
	int		m_step;

	vertex_tag& operator=(const vertex_tag &rsh);
}	vertex_t;
typedef vertex_t	*lpvertex_t;

typedef struct triangle_tag
{
	int	st_integer1;
	int	st_integer3;
	int	st_integer5;
	int		m_step;

	triangle_tag& operator=(const triangle_tag &rsh);
}	triangle_t;
typedef triangle_t	*lptriangle_t;

typedef struct shadingoption_tag
{
	UINT	st_shadingoptionID;
	int		m_step;

	shadingoption_tag& operator=(const shadingoption_tag &rsh);
}	shadingoption_t;
typedef shadingoption_t	*lpshadingoption_t;

typedef struct shadingoptions_tag
{
	shadingoption_t	st_shadingoption0;
	int		m_step;

	shadingoptions_tag& operator=(const shadingoptions_tag &rsh);
}	shadingoptions_t;
typedef shadingoptions_t	*lpshadingoptions_t;

typedef struct shading_tag
{
	float	st_float1;
	float	st_float3;
	float	st_float5;
	int	st_integer7;
	float	st_float9;
	float	st_float11;
	float	st_float13;
	CArray<shadingoptions_t,shadingoptions_t&>	st_shadingoptions15;
	int		m_step;

	shading_tag& operator=(const shading_tag &rsh);
}	shading_t;
typedef shading_t	*lpshading_t;

typedef struct light_tag
{
	coord_t	st_coord1;
	coord_t	st_coord3;
	float	st_float5;
	int		m_step;

	light_tag& operator=(const light_tag &rsh);
}	light_t;
typedef light_t	*lplight_t;

typedef struct texture_tag
{
	CString	st_string1;
	int		m_step;

	texture_tag& operator=(const texture_tag &rsh);
}	texture_t;
typedef texture_t	*lptexture_t;

typedef struct renderoption_tag
{
	int		m_step;

	renderoption_tag& operator=(const renderoption_tag &rsh);
}	renderoption_t;
typedef renderoption_t	*lprenderoption_t;

typedef struct renderoptions_tag
{
	renderoption_t	st_renderoption0;
	int		m_step;

	renderoptions_tag& operator=(const renderoptions_tag &rsh);
}	renderoptions_t;
typedef renderoptions_t	*lprenderoptions_t;

typedef struct render_tag
{
	CArray<renderoptions_t,renderoptions_t&>	st_renderoptions1;
	int		m_step;

	render_tag& operator=(const render_tag &rsh);
}	render_t;
typedef render_t	*lprender_t;

typedef struct parameter_tag
{
	UINT	st_parameterID;
	int		m_step;

	parameter_tag& operator=(const parameter_tag &rsh);
}	parameter_t;
typedef parameter_t	*lpparameter_t;

typedef struct element_tag
{
	UINT	st_elementID;
	void	*p_stelement;
	int		m_step;

	element_tag& operator=(const element_tag &rsh);
}	element_t;
typedef element_t	*lpelement_t;

typedef struct elementname_tag
{
	UINT	st_elementnameID;
	int		m_step;

	elementname_tag& operator=(const elementname_tag &rsh);
}	elementname_t;
typedef elementname_t	*lpelementname_t;

typedef struct define_tag
{
	elementname_t	st_elementname0;
	CString	st_string1;
	element_t	st_element3;
	int		m_step;

	define_tag& operator=(const define_tag &rsh);
}	define_t;
typedef define_t	*lpdefine_t;

typedef struct config_tag
{
	parameter_t	st_parameter0;
	int	st_integer2;
	int		m_step;

	config_tag& operator=(const config_tag &rsh);
}	config_t;
typedef config_t	*lpconfig_t;

typedef struct triangles_tag
{
	triangle_t	st_triangle0;
	int		m_step;

	triangles_tag& operator=(const triangles_tag &rsh);
}	triangles_t;
typedef triangles_t	*lptriangles_t;

typedef struct vertices_tag
{
	vertex_t	st_vertex0;
	int		m_step;

	vertices_tag& operator=(const vertices_tag &rsh);
}	vertices_t;
typedef vertices_t	*lpvertices_t;

typedef struct objectbase_tag
{
	CString	st_string0;
	CString	st_string2;
	CString	st_string4;
	coord_t	st_coord6;
	int		m_step;

	objectbase_tag& operator=(const objectbase_tag &rsh);
}	objectbase_t;
typedef objectbase_t	*lpobjectbase_t;

typedef struct composition_tag
{
	objectbase_t	st_objectbase0;
	CArray<vertices_t,vertices_t&>	st_vertices3;
	CArray<triangles_t,triangles_t&>	st_triangles7;
	int		m_step;

	composition_tag& operator=(const composition_tag &rsh);
}	composition_t;
typedef composition_t	*lpcomposition_t;

typedef struct sphere_tag
{
	objectbase_t	st_objectbase0;
	coord_t	st_coord1;
	float	st_float3;
	int		m_step;

	sphere_tag& operator=(const sphere_tag &rsh);
}	sphere_t;
typedef sphere_t	*lpsphere_t;

typedef struct extern_tag
{
	objectbase_t	st_objectbase0;
	CString	st_string2;
	int		m_step;

	extern_tag& operator=(const extern_tag &rsh);
}	extern_t;
typedef extern_t	*lpextern_t;

typedef struct object_tag
{
	UINT	st_objectID;
	void	*p_stobject;
	int		m_step;

	object_tag& operator=(const object_tag &rsh);
}	object_t;
typedef object_t	*lpobject_t;

typedef struct objects_tag
{
	object_t	st_object0;
	int		m_step;

	objects_tag& operator=(const objects_tag &rsh);
}	objects_t;
typedef objects_t	*lpobjects_t;

typedef struct main_tag
{
	CString	st_string0;
	CArray<config_t,config_t&>	st_config3;
	CArray<define_t,define_t&>	st_define6;
	CArray<objects_t,objects_t&>	st_objects9;
	int		m_step;

	main_tag& operator=(const main_tag &rsh);
}	main_t;
typedef main_t	*lpmain_t;



//Lex/Yacc Class Definitions

class CYaccStatus
{
	public:
		CYaccStatus() {};
		~CYaccStatus() {};

		void	Init(void);

	public:
		int		m_integer;
		float		m_float;
		CString		m_string;
		bool		m_in;
		bool		m_out;
		bool		m_sep;
		bool		m_set;
		bool		m_width;
		bool		m_height;
		bool		m_variance;
		bool		m_deflection;
		bool		m_defraction;
		bool		m_crease;
		bool		m_separate_specular;
		bool		m_none;
		bool		m_vertex;
		bool		m_shading;
		bool		m_rendering;
		bool		m_light;
		bool		m_texture;
		bool		m_triangle;
		bool		m_geometry;
		bool		m_mesh;
		bool		m_import;

		coord_t	m_stcoord;
		vertex_t	m_stvertex;
		triangle_t	m_sttriangle;
		shadingoption_t	m_stshadingoption;
		shadingoptions_t	m_stshadingoptions;
		shading_t	m_stshading;
		light_t	m_stlight;
		texture_t	m_sttexture;
		renderoption_t	m_strenderoption;
		renderoptions_t	m_strenderoptions;
		render_t	m_strender;
		parameter_t	m_stparameter;
		element_t	m_stelement;
		elementname_t	m_stelementname;
		define_t	m_stdefine;
		config_t	m_stconfig;
		triangles_t	m_sttriangles;
		vertices_t	m_stvertices;
		objectbase_t	m_stobjectbase;
		composition_t	m_stcomposition;
		sphere_t	m_stsphere;
		extern_t	m_stextern;
		object_t	m_stobject;
		objects_t	m_stobjects;
		main_t	m_stmain;
};

class CLex
{
	public:
		typedef enum
		{
			LEX_NOERR,
			LEX_ERR_SRC,
			LEX_ERR_SYN
		} LexError;

	private:
		char	buffer[BUFFER_SIZE];
		int		bufferpos;
		int		maxpos;

		bool	sourceReady;
		char	lastChar;
		CString	chunck;

		int		numLine;
		int		linePos;

		CFile	source;

		int		Parse(char &c);
		char	GetChar(void);

	public:
		CLex();
		virtual ~CLex();

		virtual CString GetChunck() { return chunck.Left(chunck.GetLength()-1); }; 
		virtual void OnError(CLex::LexError err) = 0;
		virtual bool InitLex(CString filename);
		virtual bool FinishLex(void);
		virtual int	ReadAtom(void);

		int GetNumLine(void) const { return numLine; };
		int GetLinePos(void) const { return linePos; };

};

class CYacc
{
	public:
		typedef enum
		{
			YACC_NOERR,
			YACC_ERR_LEX,
			YACC_ERR_SYN
		} YaccError;

		CYacc(CLex *parser);
		~CYacc();

		int	Analyse(void);
		void Abort(void) { abort = true; };

	protected:
		CYaccStatus	m_status;
		CLex *lex;

		virtual void OnError(CYacc::YaccError err) = 0;
		int ParseRule(void);

		virtual void Oncoord(UINT) { };
		virtual void Onvertex(UINT) { };
		virtual void Ontriangle(UINT) { };
		virtual void Onshadingoption(UINT) { };
		virtual void Onshadingoptions(UINT) { };
		virtual void Onshading(UINT) { };
		virtual void Onlight(UINT) { };
		virtual void Ontexture(UINT) { };
		virtual void Onrenderoption(UINT) { };
		virtual void Onrenderoptions(UINT) { };
		virtual void Onrender(UINT) { };
		virtual void Onparameter(UINT) { };
		virtual void Onelement(UINT) { };
		virtual void Onelementname(UINT) { };
		virtual void Ondefine(UINT) { };
		virtual void Onconfig(UINT) { };
		virtual void Ontriangles(UINT) { };
		virtual void Onvertices(UINT) { };
		virtual void Onobjectbase(UINT) { };
		virtual void Oncomposition(UINT) { };
		virtual void Onsphere(UINT) { };
		virtual void Onextern(UINT) { };
		virtual void Onobject(UINT) { };
		virtual void Onobjects(UINT) { };
		virtual void Onmain(UINT) { };

	private:
		bool abort;
		int parsecoord(int step);
		void m_oncoord(UINT RULE_ID);
		int parsevertex(int step);
		void m_onvertex(UINT RULE_ID);
		int parsetriangle(int step);
		void m_ontriangle(UINT RULE_ID);
		int parseshadingoption(int step);
		void m_onshadingoption(UINT RULE_ID);
		int parseshadingoptions(int step);
		void m_onshadingoptions(UINT RULE_ID);
		int parseshading(int step);
		void m_onshading(UINT RULE_ID);
		int parselight(int step);
		void m_onlight(UINT RULE_ID);
		int parsetexture(int step);
		void m_ontexture(UINT RULE_ID);
		int parserenderoption(int step);
		void m_onrenderoption(UINT RULE_ID);
		int parserenderoptions(int step);
		void m_onrenderoptions(UINT RULE_ID);
		int parserender(int step);
		void m_onrender(UINT RULE_ID);
		int parseparameter(int step);
		void m_onparameter(UINT RULE_ID);
		int parseelement(int step);
		void m_onelement(UINT RULE_ID);
		int parseelementname(int step);
		void m_onelementname(UINT RULE_ID);
		int parsedefine(int step);
		void m_ondefine(UINT RULE_ID);
		int parseconfig(int step);
		void m_onconfig(UINT RULE_ID);
		int parsetriangles(int step);
		void m_ontriangles(UINT RULE_ID);
		int parsevertices(int step);
		void m_onvertices(UINT RULE_ID);
		int parseobjectbase(int step);
		void m_onobjectbase(UINT RULE_ID);
		int parsecomposition(int step);
		void m_oncomposition(UINT RULE_ID);
		int parsesphere(int step);
		void m_onsphere(UINT RULE_ID);
		int parseextern(int step);
		void m_onextern(UINT RULE_ID);
		int parseobject(int step);
		void m_onobject(UINT RULE_ID);
		int parseobjects(int step);
		void m_onobjects(UINT RULE_ID);
		int parsemain(int step);
		void m_onmain(UINT RULE_ID);


		virtual int	MapTypeinteger(CString toMap) const = 0;
		virtual float	MapTypefloat(CString toMap) const = 0;
		virtual CString	MapTypestring(CString toMap) const = 0;
};

#endif
