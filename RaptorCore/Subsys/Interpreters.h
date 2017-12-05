#if !defined(__INTERPRETER_H__)
#define __INTERPRETER_H__

#include "ObjectModels/RaptorScript_lex.h"	//	CLex
#include "ObjectModels/RaptorScript_yacc.h"	//	CYacc

#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif

#include <stdlib.h>	// atoi, atof


RAPTOR_NAMESPACE_BEGIN

class CPersistence;
class CShader;
class CObject3D;

//////////////////////////////////////////////////////////////////////
// Frames structures
//////////////////////////////////////////////////////////////////////
typedef enum
{
	PARAM0,
	PARAM1,
	PARAM2
} PARAMETER;

typedef enum
{
	ROTATEX,
	ROTATEY,
	ROTATEZ,
	TRANSLATE,
	PUSH,
	POP,
	RENDER,
	ENABLETEXTURE,
	DISABLETEXTURE,
	ENABLELIGHTING,
	DISABLELIGHTING
} ACTION;

typedef float (RAPTOR_FASTCALL *p_FnTimeFunction)(float t,float p1,float p2,float p3,float p4);

typedef struct TIMEFUNCTION_tag
{
	string				name;
	p_FnTimeFunction	function;	//	function == NULL => float value in p1
	float				p1;
	float				p2;
	float				p3;
	float				p4;
} TIMEFUNCTION;
typedef TIMEFUNCTION* LPTIMEFUNCTION;

typedef struct TARGETACTION_tag
{
	string			target;
	ACTION			action;
	LPTIMEFUNCTION	tFn1;
	LPTIMEFUNCTION	tFn2;
	LPTIMEFUNCTION	tFn3;
	CObject3D		*obj;	//	obj == NULL => OpenGL
} TARGETACTION;
typedef TARGETACTION* LPTARGETACTION;

typedef struct FRAMESEQUENCE_tag
{
	int		nbFrames;
	vector<LPTIMEFUNCTION>	timeFunctions;
	vector<LPTARGETACTION> targetActions;
} FRAMESEQUENCE;
typedef FRAMESEQUENCE* LPFRAMESEQUENCE;


//////////////////////////////////////////////////////////////////////
// This class implements the lexical analyser
//	and for this implements the virtual OnError
//	to handle file parsing problems
//////////////////////////////////////////////////////////////////////
class CLexical:public CLex
{
public:
	CLexical():displayError(true) {};
	~CLexical() {};

	virtual void OnError(CLex::LexError err);

    const string& getErrorMsg(void) const { return msg; }

    void DisplayError(bool display) { displayError = display; }


private:
	bool	displayError;
	string	msg;
};


//////////////////////////////////////////////////////////////////////
// This class implements the syntaxic analyser
//	and for this implements all the virtual
//	functions that handle structures parsed
//	in the source script
//////////////////////////////////////////////////////////////////////

#define MAX_LOCAL_VAR 36

class CMicroYacc:public CYacc
{
public:
	CMicroYacc(CLexical *parser)
		:CYacc(parser),
		displayError(true),displayHelp(false),clear(false),msg(""),
		error(MICROYACC_NO_ERROR),m_currentObject(NULL),
		ta(NULL),fs(NULL),tf(NULL)
	{};

	~CMicroYacc(){};

	typedef enum 
	{
		MICROYACC_NO_ERROR = CYacc::YACC_NOERR,
		UNKNOWN_ID,
		TOO_MANY_VECTORS,
		TOO_MANY_MATRIX,
		INVALID_OPERANDS,
		OUT_OF_RANGE,
		INVALID_SWIZZLE,
		WRONG_PARAM_NUMBER,
		NB_MICROYACC_ERROR
	} MicroYaccError;

	void InitAnalyse(void);
	
	virtual void OnError(CYacc::YaccError err);

    virtual void Onhelp(unsigned int RULE_ID);
	
	void Onload3DS(unsigned int RULE_ID);
	void Onvertexprogram(unsigned int RULE_ID);
	void Onvertexprogramtarget(unsigned int RULE_ID);
	void Ontextureshadertarget(unsigned int RULE_ID);
	void Onoperation(unsigned int RULE_ID);
	void Onoptionnalswizzle(unsigned int RULE_ID);
	void Onswizzle(unsigned int RULE_ID);
	void Onoperand(unsigned int RULE_ID);
	void Ondeclaration(unsigned int RULE_ID);
	void Onprogramparameter_listitem(unsigned int RULE_ID);
	void Onprogramparameter_lastitem(unsigned int RULE_ID);
	void Onselection(unsigned int RULE_ID);
    void Onscaleobject(unsigned int RULE_ID);
    void Ontranslateobject(unsigned int RULE_ID);
    void Ontranslateobjectabsolute(unsigned int RULE_ID);
    void Onvisibleobject(unsigned int RULE_ID);
    void Onrotateobject(unsigned int RULE_ID);
	void Onclear(unsigned int RULE_ID);

    const string& getErrorMsg(void) const { return msg; }

    void DisplayError(bool display) { displayError = display; }
    bool DisplayHelp(void) 
    { bool h = displayHelp; displayHelp = false; return h; }
	bool Clear(void) 
    { bool h = clear; clear = false; return h; }

private:
	bool	displayError;
    bool    displayHelp;
	bool	clear;
	string	msg;

	MicroYaccError	error;

	string			shaderSrc;
	string			instructionSrc;
	CShader			*shader;
    CObject3D				*m_currentObject;

	TIMEFUNCTION	*tf;
	TARGETACTION	*ta;
	FRAMESEQUENCE	*fs;

	typedef struct local_vat_tag
	{
		string str;
		unsigned int	type;
		unsigned int	reg;
	} local_var_t;
	local_var_t	regMap[MAX_LOCAL_VAR];
	local_var_t	operand0;
	local_var_t	operand1;
	local_var_t	operand2;

	string		swizzle;
	unsigned int		swizzleId;

	CShader		*getShader(void);
	void GenerateTextureShader(void);

	//	functions for type mapping declared in Raptor.lex
	virtual int	MapTypeinteger(string toMap) const { return atoi(toMap.data()); };
	virtual float	MapTypefloat(string toMap) const 	{ return (float)(atof(toMap.data())); };
	virtual string	MapTypestring(string toMap) const;
	virtual string	MapTypeidentifier(string toMap) const 	{ return toMap; }
};

RAPTOR_NAMESPACE_END

#endif //	__INTERPRETER_H__

