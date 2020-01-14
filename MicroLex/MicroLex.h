/***************************************************************************/
/*                                                                         */
/*  Microlex.h                                                             */
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


#if !defined(AFX_MICROLEX_H__51917955_C7D2_4335_8040_5B32D85D6AAE__INCLUDED_)
#define AFX_MICROLEX_H__51917955_C7D2_4335_8040_5B32D85D6AAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>

//separators
#define NEW_LINE		10
#define	CARRIAGE_RETURN	13
#define	TAB				9
#define SPACE			32

#define IS_SEPARATOR(c) \
	((c==NEW_LINE)||(c==CARRIAGE_RETURN)||(c==TAB)||(c==SPACE))


#define BUFFER_SIZE	256

class atom;
class rule;
typedef struct _node_tag node;
typedef struct _rule_selector_tag rule_selector;

typedef struct _branch_tag
{
	string			branchChars;
	node			*nextNode;
	vector<atom*>	rulesSrc;
} branch;

typedef struct _node_tag
{
	vector<branch*>	branches;
	unsigned int	terminate;
	unsigned int	length;		//	the length of the string currently parsed
} node;


//	TODO:
//		Manage atom deletion when not added 
//		to the global singleton
class atom
{
public:
	atom(const string &name,unsigned int id,rule* macroAtom=NULL,const string &mapType="");

	const string&	Name(void) const { return m_name; };
	unsigned int	ID(void) const { return m_atomID; };
	rule*			GetRule(void) const { return m_macroAtom; };
	unsigned int	NbValidChars(void) const { return m_validChars.size(); };
	string			GetValidChars(size_t pos);
	void			SetValidChars(size_t pos,const string& chars);
	void			AddValidChars(const string& chars);
	string			MapType(void) const { return m_mapType; };
	void			MapType(const string& map) { m_mapType = map; };

	void			Display(void);

// Static management of global atoms.
// Better here to acces private atom data
// instead of a dedicated global class
// with public access
static bool			AddAtom(atom *a);
static atom*		GetAtom(unsigned int pos);
static unsigned int	GetNbAtoms(void) { return m_atoms.size(); }
static atom*		FindAtom(const string& name);
static void			DestroyAtoms(void);

private:
	string			m_name;
	vector<string>	m_validChars;
	unsigned int	m_atomID;
	rule			*m_macroAtom;
	string			m_mapType;

	~atom();

	static vector<atom*>	m_atoms;
};



class rule
{
public:
	rule(const string &name,unsigned int id,bool sequence,bool forward);

	const string	Name(void) const { return m_name; };
	void			ReName(const string& name) { m_name = name; };
	unsigned int	ID(void) const { return m_ruleID; };
	void			Sequence(bool sequence) { m_isSequence = sequence; };
	bool			IsSequence(void) const { return m_isSequence; };
	void			Forward(bool forward) { m_forward = forward; };
	bool			IsForward(void) const { return m_forward; };
	bool			IsLoop(void) const { return m_loop; };
	void			Loop(bool loop) { m_loop = loop; };
	bool			AllowEmpty(void) const { return m_allowEmpty; };
	void			AllowEmpty(bool allowempty) { m_allowEmpty = allowempty; };
	unsigned int	GetNbAtoms(void) const { return m_atomList.size(); };
	atom*			GetAtom(unsigned int pos);
	void			AddAtom(atom* a);
	bool			RemoveAtom(unsigned int pos);
    void    		SetAtomOccurences(unsigned int pos,int occurences);
    int    		    GetAtomOccurences(unsigned int pos);

	void			Display(void);

// Static management of global rules.
// Better here to acces private rule data
// instead of a dedicated global class
// with a public access
static int			AddRule(rule *r);
static rule*		GetRule(unsigned int pos);
static unsigned int	GetNbRules(void) { return m_rules.size(); }
static rule*		FindRule(const string& name);
static void			DestroyRules(void);


private:
	string			m_name;
	unsigned int	m_ruleID;

    typedef struct atom_occurence_t
    {
        atom*   pAtom;
        int     occurences;
    } atom_occurence;
	vector<atom_occurence>	m_atomList;
	bool			m_isSequence;	//	the rule is a sequence of rules or atoms
	bool			m_loop;			//	the rule has a loop ( must be a sequence )
	bool			m_forward;		//	the rule has a forward declaration
	bool			m_allowEmpty;	//	the rule accepts empty tag

	~rule();

	static vector<rule*>	m_rules;
};

typedef struct _rule_selector_tag
{
	unsigned int	selectorID;
	rule			*selector;
	vector<rule_selector*>	ruleSelectors;
	vector<rule*>	selectedRules;
	bool			rollback;
} rule_selector;


class StreamManager
{
public:
	StreamManager();
	~StreamManager();

	bool	PushSourceFile(const string& fname);
	void	PopSourceFile();
	int		ParseInclude(char &car);
	char	GetChar(void);
	char	ReadToken(char delim,string &token);

	int		GetNumLine(void) const { return numline; };
	int		GetLinePos(void) const { return linepos; };

	int OpenMicroLexFile(const string& fname);
	string GetFileName(unsigned int file_ID);
	void CloseMicroLexFiles(void);
	void WriteData(unsigned int file_ID,const char *data);

private:
	int		numline;
	int		linepos;
	int		current_source;

	typedef struct source_stream_t source_stream;
	vector<source_stream*>	sources;

	typedef struct microlex_file_t microlex_file;
	vector<microlex_file*>	microlex_files;
};


class MicroLexConfig
{
public:
	~MicroLexConfig() {};

	static MicroLexConfig* GetConfig(void);

	void Usage(void);
	int InitConfig(int argc,char* argv[]);

	bool Verbose(void) const { return verbose; };
	bool RenameClasses(void) const { return renameClasses; };
	string GetInput(void) const { return input; };
	string GetOutput(void) const { return output; };
	string GetYaccClass(void) const { return yaccclass; };
	string GetLexClass(void) const { return lexclass; };

private:
	bool		verbose;
	string		lexclass;
	string		yaccclass;
	bool		renameClasses;
	string		output;
	string		input;

	static MicroLexConfig*	m_pConfig;

	MicroLexConfig();
};

#endif // !defined(AFX_MICROLEX_H__51917955_C7D2_4335_8040_5B32D85D6AAE__INCLUDED_)
