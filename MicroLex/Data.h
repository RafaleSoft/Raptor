//
//	This file defines all static data dumped into the parser
//
//
static const char *HEADER =
"//////////////////////////////////////////////////////////////////////////\n\
//	This file is generated MicroLex application from Rafale Soft Inc. (c)\n\
//\n\
//	Do not modify this file, your changes would be lost on next generation\n\
//	This file defines and implements a 'lex like' parser as well as a\n\
//	'yacc like' interpreter to use with your application that requires \n\
//	a small language to format your data files in an expandable way\n\
///////////////////////////////////////////////////////////////////////////\n\
\n\n";

static const char *ENDPROTECTHEADER =
"#if !defined(__LEX_H__)\n\
#define __LEX_H__\n\n";

static const char *LEXCLASS = 
"class CLex\n\
{\n\
	public:\n\
		typedef enum\n\
		{\n\
			LEX_NOERR,\n\
			LEX_ERR_SRC,\n\
			LEX_ERR_SYN,\n\
			LEX_ERR_INT\n\
		} LexError;\n\n\
		typedef struct lex_chunck_t\n\
		{\n\
			string chunck;\n\
			unsigned int	id;\n\
		} lex_chunck;\n\n\
	public:\n\
		CLex();\n\
		virtual ~CLex();\n\n\
		virtual bool InitLex(string filename);\n\
		virtual bool InitLex(const char* stream);\n\
		virtual bool FinishLex(void);\n\n\
		virtual string GetChunck() const { return chunck.substr(0,chunck.size()-1); }; \n\
		virtual unsigned int GetID() const { return ID; };\n\
		virtual int	ReadAtom(void);\n\n\
		virtual void OnError(CLex::LexError err) = 0;\n\
		virtual void OnComment(void) {};\n\n\
		int GetNumLine(void) const { return numLine; };\n\
		int GetLinePos(void) const { return linePos; };\n\n\
		void Push(void);\n\
		void Pop(void);\n\
		bool IsInRollback() const { return rollback; }\n\
		int Rollback(void);\n\n\
	private:\n\
		// lex buffers\n\
		typedef vector<lex_chunck>	prefetchPool;\n\n\
		prefetchPool							reject;\n\
		vector<prefetchPool*>	prefetchPools;\n\n\
		char							*buffer;\n\
		int								bufferpos;\n\
		int								maxpos;\n\n\
		//	lex source\n\
		ifstream							source;\n\
		bool							sourceReady;\n\
		int								numLine;\n\
		int								linePos;\n\n\
		//	current parse process \n\
		int								index;\n\
		bool							rollback;\n\
		char							lastChar;\n\
		unsigned int					ID;\n\
		string							chunck;\n\n\
		//	internal management\n\
		int		Parse(char &c);\n\
		bool	IsComment(unsigned int id);\n\
		char	GetChar(void);\n\n\
};\n\n";

static const char *BUFFER =
"// continous char provider from file\n\
char CLex::GetChar(void)\n\
{\n\
	char c;\n\n\
	if (bufferpos == maxpos)\n\
	{\n\
		if ((source.is_open()) && (source.good()) && (!source.eof()))\n\
		{\n\
			source.read(buffer,BUFFER_SIZE);\n\
			maxpos = source.gcount();\n\
			bufferpos = 0;\n\n\
			if (maxpos == 0)\n\
			{\n\
				bufferpos = -1;\n\
				c = EOF;\n\
			}\n\
			else\n\
				c = buffer[bufferpos++];\n\
		}\n\
		else\n\
		{\n\
			bufferpos = -1;\n\
			c = EOF;\n\
		}\n\
	}\n\
	else\n\
		c = buffer[bufferpos++];\n\n\
	if (c == NEW_LINE)\n\
	{\n\
		numLine++;\n\
		linePos = 0;\n\
	}\n\
	else\n\
		linePos++;\n\n\
	chunck+=c;\n\n\
	return c;\n\
}\n\n";

static const char *INIT =
"bool CLex::InitLex(string filename)\n\
{\n\
	index = 0;\n\
	rollback = false;\n\
	numLine = 1;\n\
	linePos = 1;\n\
	ID = 0;\n\
	chunck = \"\";\n\n\
	if (buffer != NULL)\n\
		delete [] buffer;\n\
	buffer = new char[BUFFER_SIZE];\n\n\
	source.open(filename.data(),ios_base::in);\n\
	if (!source.is_open() || !source.good())\n\
	{\n\
		OnError(CLex::LEX_ERR_SRC);\n\
		sourceReady = false;\n\
	}\n\
	else\n\
	{\n\
		lastChar = GetChar();\n\
		sourceReady = true;\n\
	}\n\
	return sourceReady;\n\
}\n\n\
bool CLex::InitLex(const char* stream)\n\
{\n\
	index = 0;\n\
	rollback = false;\n\
	numLine = 1;\n\
	linePos = 1;\n\
	ID = 0;\n\
	chunck = \"\";\n\n\
	maxpos = strlen(stream);\n\
	if (buffer != NULL)\n\
		delete [] buffer;\n\
	buffer = new char[maxpos+1];\n\n\
	strcpy(buffer,stream);\n\
	bufferpos = 0;\n\
	lastChar = GetChar();\n\
	sourceReady = true;\n\
	return sourceReady;\n\
}\n\n";

static const char *FINISH =
"bool CLex::FinishLex(void)\n\
{\n\
	try\n\
	{\n\
		rollback = false;\n\
		if (prefetchPools.size() > 0)\n\
		{\n\
			prefetchPool *p = prefetchPools[0];\n\
			prefetchPools.erase(prefetchPools.begin());\n\
			delete p;\n\
		}\n\n\
		prefetchPools.clear();\n\
		reject.clear();\n\
		sourceReady = false;\n\
		bufferpos = BUFFER_SIZE;\n\
		maxpos = BUFFER_SIZE;\n\
		lastChar = 0;\n\
		if (source.good() && source.is_open())\n\
			source.close();\n\
		return true;\n\
	}\n\
	catch(...)\n\
	{\n\
		OnError(CLex::LEX_ERR_SRC);\n\
		return false;\n\
	}\n\
}\n\n";

static const char *READATOM =
"int CLex::ReadAtom()\n\
{\n\
	if (!sourceReady)\n\
		return -1;\n\
\n\
	int size = reject.size();\n\
\n\
	if (size > 0)\n\
	{\n\
		size--;\n\
		lex_chunck lc = reject[size];\n\
		reject.pop_back();\n\
		chunck = lc.chunck;\n\
		ID = lc.id;\n\
	}\n\
	else\n\
	{\n\
		do\n\
		{\n\
			while IS_SEPARATOR(lastChar)\n\
				lastChar = GetChar();\n\
\n\
			chunck.erase();\n\
			chunck+=lastChar;\n\
\n\
			ID = Parse(lastChar);\n\
		} while (IsComment(ID));\n\
	}\n\
\n\
	size = prefetchPools.size();\n\
	if (size > 0)\n\
	{\n\
		lex_chunck lc;\n\
		lc.chunck = chunck;\n\
		lc.id = ID;\n\
\n\
		prefetchPools[size-1]->push_back(lc);\n\
	}\n\
\n\
	return ID;\n\
}\n\n";

static const char *PREFETCH =
"void CLex::Push(void)\n\
{\n\
	lex_chunck lc;\n\
	lc.chunck = chunck;\n\
	lc.id = ID;\n\n\
	rollback = false;\n\
	prefetchPool *p = new prefetchPool;\n\
	p->push_back(lc);\n\
	prefetchPools.push_back(p);\n\
};\n\n\
int CLex::Rollback(void)\n\
{\n\
	rollback = true;\n\n\
	int size = prefetchPools.size();\n\
	if (size > 0)\n\
	{\n\
		prefetchPool *p = prefetchPools[size-1];\n\
		prefetchPools.pop_back();\n\
\n\
		int sz = p->size();\n\
		while (sz > 0)\n\
		{\n\
			sz--;\n\
			lex_chunck lc = p->at(sz);\n\
			reject.push_back(lc);\n\
		}\n\
		delete p;\n\
	}\n\
	else\n\
	{\n\
		OnError(CLex::LEX_ERR_INT);\n\
		return -1;\n\
	}\n\n\
	return ID;\n\
};\n\n\
void CLex::Pop(void)\n\
{\n\
	int size = prefetchPools.size();\n\
\n\
	if (size > 0)\n\
	{\n\
		prefetchPool *p = prefetchPools[size-1];\n\
		prefetchPools.pop_back();\n\
\n\
		delete p;\n\
	}\n\
	else\n\
	{\n\
		OnError(CLex::LEX_ERR_INT);\n\
	}\n\
}\n\n";


static const char *DEFINE =
"\n\n#define PARSE_ATOM0(a,r)\\\n\
{\\\n\
	result = lex->ReadAtom();\\\n\
	if (result != ATOM_##a##_ID)\\\n\
		return result;\\\n\
	m_on##r(result);\\\n\
	if (abort) return -1;\\\n\
}\\\n\n\
#define PARSE_ATOM(a,r)\\\n\
{\\\n\
	result = lex->ReadAtom();\\\n\
	if (result != ATOM_##a##_ID)\\\n\
	{\\\n\
		OnError(YACC_ERR_SYN);\\\n\
		return -1;\\\n\
	}\\\n\
	m_on##r(result);\\\n\
	if (abort) return -1;\\\n\
}\\\n\n\
#define PARSE_SUB_RULE0(a,r)\\\n\
{\\\n\
	result = parse##a(0);\\\n\
	if (result != RULE_##a##_ID)\\\n\
		return result;\\\n\
	m_on##r(result);\\\n\
	if (abort) return -1;\\\n\
}\\\n\n\
#define PARSE_SUB_RULE(a,r)\\\n\
{\\\n\
	r##_t tmp_rule;\\\n\
	a##_t tmp_subrule;\\\n\
	tmp_rule = status->m_st##r;\\\n\
	tmp_subrule = status->m_st##a;\\\n\
	result = parse##a(0);\\\n\
	if (result != RULE_##a##_ID)\\\n\
	{\\\n\
		OnError(YACC_ERR_SYN);\\\n\
		return -1;\\\n\
	}\\\n\
	status->m_st##r = tmp_rule;\\\n\
	m_on##r(result);\\\n\
	status->m_st##a = tmp_subrule;\\\n\
	if (abort) return -1;\\\n\
}\\\n\n\
#define PARSE_LOOP_RULE(a,r,r2)\\\n\
{\\\n\
	int currentStep = status->m_st##r.m_step;\\\n\
	while ((result = parse##r2(0)) == RULE_##r2##_ID)\\\n\
	{\\\n\
		status->m_st##r.m_step = currentStep;\\\n\
		m_on##r(result);\\\n\
		if (abort) return -1;\\\n\
	}\\\n\
	if ((result != ATOM_eof_ID) && (result != ATOM_##a##_ID))\\\n\
	{\\\n\
		OnError(YACC_ERR_SYN);\\\n\
		abort = true;\\\n\
		return -1;\\\n\
	}\\\n\
	m_on##r(result);\\\n\
	if (abort) return -1;\\\n\
}\\\n\n\
#define PARSE_LOOP_RULE_FINAL(r,r2)\\\n\
{\\\n\
	int currentStep = status->m_st##r.m_step;\\\n\
	while ((result = parse##r2(0)) == RULE_##r2##_ID)\\\n\
	{\\\n\
		status->m_st##r.m_step = currentStep;\\\n\
		m_on##r(result);\\\n\
		if (abort) return -1;\\\n\
	}\\\n\
    lex->Push();\\\n\
	lex->Rollback();\\\n\
	m_on##r(result);\\\n\
	if (abort) return -1;\\\n\
}\\\n\n";



static const char *YACC = 
"class CYacc\n\
{\n\
	public:\n\
		typedef enum\n\
		{\n\
			YACC_NOERR,\n\
			YACC_ERR_LEX,\n\
			YACC_ERR_SYN\n\
		} YaccError;\n\n\
		CYacc(CLex *parser);\n\
		~CYacc();\n\n\
		int	Analyse(void);\n\
		void Abort(void) { abort = true; };\n\n\
	protected:\n\
		CYaccStatus	*status;\n\
		CLex		*lex;\n\n\
		virtual void OnError(CYacc::YaccError err) = 0;\n\
		int ParseRule(void);\n\n";

static const char *ANALYSE =
"int CYacc::Analyse(void)\n\
{\n\
	int result=0;\n\
	status->Init();\n\
\n\
	result = parsemain(0);\n\
\n\
	if(result!=RULE_main_ID)\n\
		result = -1;\n\
\n\
	return result;\n\
}\n";



