/***************************************************************************/
/*                                                                         */
/*  MicroLex.cpp                                                           */
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


#include "StdAfx.h"
#include "MicroLex.h"
#include "Automat.h"

#include <stdio.h>	// EOF,...

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

vector<unsigned int>		comments;
StreamManager	stream;
CAutomat	automat;

// generation options
	
extern unsigned int ATOM_ID;

extern void WriteHeader(StreamManager& stream,unsigned int filelex,unsigned int fileyacc,unsigned int filetree);
extern void WriteCode(StreamManager& stream,unsigned int filelex,unsigned int fileyacc,unsigned int filetree);
extern int ParseAtom(StreamManager& stream,char &car);
extern int MapToType(StreamManager& stream,char &car);
extern int MapToComment(StreamManager& stream,char &car);
extern int ParseRule(StreamManager& stream,char &car,bool isSequence);
extern int ParsePredecl(StreamManager& stream,char &c);
extern void delete_branch(branch *b,node *n);
extern void delete_node(node *n);

using namespace std;

//#define BUILD_LEX_YACC 
#ifdef BUILD_LEX_YACC
	#include "RaptorScript_lex.cpp"
	#include "RaptorScript_yacc.cpp"
	#include "RaptorScript_tree.cpp"

	class CLexical:public CLex
	{
	public:
		CLexical() {};
		~CLexical() {};

		virtual void OnError(CLex::LexError err)
		{
			cout << "Error: " << err;
			cout << " on line " << GetNumLine();
			cout << " at pos " << GetLinePos() << endl;
		};
	};

	class CMicroYacc:public CYacc
	{
	public:
		CMicroYacc(CLexical *parser):CYacc(parser){};
		~CMicroYacc(){};

		void OnError(CYacc::YaccError /*err*/)
		{
			cout << "Syntax error";
			cout << " on line " << lex->GetNumLine();
			cout << " at pos " << lex->GetLinePos() << endl;
		};
		int MapTypeinteger(const std::string& str) const { return atoi(str.data()); };
		float MapTypefloat(const std::string& str) const { return atof(str.data()); };
		string MapTypestring(const std::string& str) const { return str; };
		string MapTypeidentifier(const std::string& str) const { return str; };
	};

	CLexical lex;
	CMicroYacc yacc(&lex);
#endif

int ParseFile(void)
{
	//	Add predefined atoms : EOF
	atom *a = new atom("eof",0xffffffff,(rule*)0,"");
	string _eof;
	_eof += ((char)EOF);
	a->AddValidChars(_eof);
	atom::AddAtom(a);

	int err = 0;
	char c = stream.GetChar();
	
	while ((err==0)&&(c != EOF))
	{
		//	trail seperators
		while (IS_SEPARATOR(c))
			c = stream.GetChar();

		//	comments
		if ((c=='#')||(c=='/'))
		{
			string include;

			while ((c!=NEW_LINE)&&(c!=EOF))
			{
				c = stream.GetChar();
				include += c;
				if (include == "include")
				{
					c = stream.GetChar();
					while (IS_SEPARATOR(c))
						c = stream.GetChar();
					err = stream.ParseInclude(c);
				}
			}
		}
		else if (c!=EOF)
		{		
			// read line type
			string type;
			type += c;
			stream.ReadToken(':',type);
			
			if (type =="atom")
				err = ParseAtom(stream,c);
			else if (type =="sequence")
				err = ParseRule(stream,c,true);
			else if (type =="alternate")
				err = ParseRule(stream,c,false);
			else if (type =="forward")
				err = ParsePredecl(stream,c);
			else if (type =="map")
				err = MapToType(stream,c);
			else if (type == "comment")
				err = MapToComment(stream,c);
			else 
			{
				cout << "Error: unrecognized token '" << type.data() << "'" << endl;
				err = -1;
			}
		}
	}

	return err;
}



int BuildParser(const std::string& output)
{
	rule *mainRule = rule::FindRule("main");
	if (mainRule == NULL)
	{
		cout << "Main rule named main is not defined" << endl;
		return 3;
	}
	else
	{
		if (MicroLexConfig::GetConfig()->RenameClasses())
		{
			string mainRuleName = "main" + MicroLexConfig::GetConfig()->GetYaccClass();
			mainRule->ReName(mainRuleName.data());
			cout << "Main rule named main is renamed : " << mainRuleName.data() << endl;
		}
	}

	unsigned int headerlex_ID;
	unsigned int codelex_ID; 
	unsigned int headeryacc_ID;
	unsigned int codeyacc_ID; 
	unsigned int headertree_ID;
	unsigned int codetree_ID;

	cout << "   Creating source files: " << endl;
	
	string fname = output+"_lex.cpp";
	cout << "		- " << fname << endl;
	codelex_ID = stream.OpenMicroLexFile(fname);
	if (codelex_ID < 0)
		return 2;
	
	fname = output+"_yacc.cpp";
	cout << "		- " << fname << endl;
	codeyacc_ID = stream.OpenMicroLexFile(fname);
	if (codeyacc_ID < 0)
		return 2;
	
	fname = output+"_tree.cpp";
	cout << "		- " << fname << endl;
	codetree_ID = stream.OpenMicroLexFile(fname);
	if (codetree_ID < 0)
		return 2;

	cout << "   Creating header files: " << endl;

	fname = output+"_lex.h";
	cout << "		- " << fname << endl;
	headerlex_ID = stream.OpenMicroLexFile(fname);
	if (headerlex_ID < 0)
		return 2;

	fname = output+"_yacc.h";
	cout << "		- " << fname << endl;
	headeryacc_ID = stream.OpenMicroLexFile(fname);
	if (headeryacc_ID < 0)
		return 2;

	fname = output+"_tree.h";
	cout << "		- " << fname << endl;
	headertree_ID = stream.OpenMicroLexFile(fname);
	if (headertree_ID < 0)
		return 2;

	cout << "   Generating headers: " << endl;
	WriteHeader(stream,headerlex_ID,headeryacc_ID,headertree_ID);
	cout << "   Generating code: " << "..." << endl;
	WriteCode(stream,codelex_ID,codeyacc_ID,codetree_ID);

	stream.CloseMicroLexFiles();

	return 0;
}

int main(int argc, char* argv[], char* envp[])
{
	int nRetCode = 0;

#ifdef BUILD_LEX_YACC 
    string filename = "RaptorScriptTest.lex";
    lex.InitLex(filename);
    yacc.Analyse();
#endif

	cout << "Micro lex parser source generator" << endl;
	cout << "by Rafale Soft Inc.(c)" << endl;
	cout << "Version : 1.2.0.0" << endl << endl;

	if (argc == 1)
		MicroLexConfig::GetConfig()->Usage();

	if (MicroLexConfig::GetConfig()->InitConfig(argc,argv) > 0)
	{
		string input = MicroLexConfig::GetConfig()->GetInput().data();

		if (input.empty())
		{
			cout << "No input file specified, nothing to do." << endl;
			nRetCode = 1;
		}
		else if (false == stream.PushSourceFile(input))
		{
			nRetCode = 2;
		}
		else
		{
			cout << "Compiling " << input << " ..." << endl << endl;
			nRetCode = ParseFile();

			if (nRetCode != 0)
			{
				cout	<< "Parse error in line " << stream.GetNumLine() << " at pos " 
						<< stream.GetLinePos() << endl << endl;
			}

			if (nRetCode == 0)
				nRetCode = automat.BuildAutomat();
			if (nRetCode == 0)
				nRetCode = BuildParser(MicroLexConfig::GetConfig()->GetOutput().data());
		}

		cout << endl;
		cout << "clearing memory" << endl;
		rule::DestroyRules();
		atom::DestroyAtoms();
		automat.DestroyBranches();
	}
	else
		nRetCode = -1;
	
	if (nRetCode == 0)
		cout << "Microlex job: no errors!" << endl;
	else
		cout << "Microlex job not done: errors found!" << endl;

//	for (int ii=0;ii<12;ii++)
//		cout << parser[ii].str << " " << parser[ii].next_node << endl;

	//lex.InitLex("2+3*4");
	//yacc.Analyse();

	return nRetCode;
}





