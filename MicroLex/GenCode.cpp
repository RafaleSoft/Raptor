#include "StdAfx.h"
#include "MicroLex.h"

#include "Data.h"
#include "Automat.h"

#include <stdio.h>	// sprintf, ...
#include <string.h>	// strlen, ...

extern vector<unsigned int>	comments;

extern rule_selector *BuildRuleSelectors(rule *r);
extern void DeleteRuleSelectors(rule_selector *r);
extern vector<string> *BuildRulePath(atom *start, atom* a,unsigned int id);
extern string IdToString(unsigned int id);
extern CAutomat automat;

#include <strstream>
using namespace std;

static char FORMAT_BUFFER[1024];

#define		WRITE	stream.WriteData(file_ID,str.data());

void Replace(string &str,const char*pattern,const string &newPattern)
{
	unsigned int pos = 0;
	
	while (string::npos != (pos = str.find(pattern,pos)))
	{
		str.replace(pos,strlen(pattern),newPattern);
		pos += newPattern.size();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	Generates public defines and types to use during parsing in user
//	derivated classes
//
//
void WriteRuleDefines(StreamManager& stream,unsigned int file_ID)
{
	string str;
	unsigned int	currentID = 0;

	str="\n\n#include <vector>\n#include <fstream>\n\nusing namespace std;\n\n";
	WRITE;

	//	Writes the specific atom for the empty
	//	tag of atom only alternate rules
	str="\n#define ATOM_empty_ID		0\n\n";
	WRITE;

	unsigned int i = 0;
	for (i=0;i<atom::GetNbAtoms();i++)
	{
		atom *a = atom::GetAtom(i);
		//	to avoid already defined symbols due to optional
		//	characters that generates two rules with the same ID
		if (a->ID()>currentID)
		{
			SPRINTF(FORMAT_BUFFER,"#define ATOM_%s_ID		%d\n",a->Name().data(),a->ID());
			str = FORMAT_BUFFER;
			WRITE;
			currentID = a->ID();
			if (currentID == ~0)
				currentID = 0;
		}
	}
	currentID = 0;
	for (i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);
		//	to avoid already defined symbols due to optional
		//	characters that generates two rules with the same ID
		if (r->ID()>currentID)
		{
			SPRINTF(FORMAT_BUFFER, "#define RULE_%s_ID		%u\n", r->Name().data(), r->ID());
			str = FORMAT_BUFFER;
			WRITE;
			currentID = r->ID();
		}
	}

	str="\n#define BUFFER_SIZE 256\n\n";
	WRITE;

	str="//separators\n\
#define NEW_LINE		10\n\
#define	CARRIAGE_RETURN	13\n\
#define	TAB				9\n\
#define SPACE			32\n\n\
#define IS_SEPARATOR(c) \\\n\
	((c==NEW_LINE)||(c==CARRIAGE_RETURN)||(c==TAB)||(c==SPACE))\n\n\n";
	WRITE;
}


////////////////////////////////////////////////////////////////////////////////
//
//	Generates public structures to use during parsing in user
//	derivated classes. All these structures will store the file
//	parsed and are gathered in the YaccStatus. The user can refer
//	to them anytime during parsing to implement Yacc's callbacks.
//
void WriteTypes(StreamManager& stream,unsigned int file_ID)
{
	string str="\n\n#include <vector>\n#include <string>\n\nusing namespace std;\n\n";
	WRITE;

	str="\n\n//Syntaxic structures typedefs\n";
	WRITE;

	for (unsigned int i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);
		if (r->IsForward()) continue;

		str="typedef struct "+r->Name()+"_tag\n{\n";
		WRITE;

		atom *a;

		if (r->IsSequence())
		{
			for (unsigned int j=0;j<r->GetNbAtoms();j++)
			{
				ostrstream id;
				a = r->GetAtom(j);
				if (!a->MapType().empty())
				{
					id << j << ends;
					if (r->GetAtomOccurences(j) > 1)
						str = string("	vector<")+a->MapType()+">	st_"+a->Name()+id.str()+";\n";
					else
						str = string("	")+a->MapType()+"	st_"+a->Name()+id.str()+";\n";
					WRITE;
				}
				else if (a->GetRule() != NULL)
				{
					rule *rr = a->GetRule();
					id << j << ends;
					const string &n = rr->Name();

					if (rr->IsLoop())
						str="	vector<"+n+"_t,allocator<"+n+"_t> >	";
					else
						str="	"+n+"_t	";

					str+="st_"+n+id.str()+";\n";
					WRITE;
				}
			}
		}
		else
		{
			str="	unsigned int	st_"+r->Name()+"ID;\n";
			WRITE;

			//	in an alternate, each rule must be of the same kind:
			//	an atom or a rule. So let's test just the first one

			a = r->GetAtom(0);
			if (a->GetRule() != NULL)
			{
				//	it is a rule, generate a pointer for
				//	the alternate structure
				str = "	void	*p_st"+r->Name()+";\n";
				WRITE;
			}
			else if (!a->MapType().empty())
			{
				//	it is an atom, generate a struct if atoms
				//	are mapped to a type.
				//	( canot use a union because an atom can be
				//	mapped to a type with a copy constructor )
				str = "	struct "+r->Name()+"_value_t\n	{\n";
				WRITE;
				for (unsigned int j=0;j<r->GetNbAtoms();j++)
				{
					a = r->GetAtom(j);
					SPRINTF(FORMAT_BUFFER, "		%s %s_%u;\n", a->MapType().data(), a->Name().data(), j);
					str = FORMAT_BUFFER;
					WRITE;
				}
				SPRINTF(FORMAT_BUFFER, "	} %s_value;\n", r->Name().data());
				str = FORMAT_BUFFER;
				WRITE;
			}
		}

		//	structure step & assignment operator
		str="	int		m_step;\n\n";
		WRITE;
		str="	"+r->Name()+"_tag& operator=(const "+r->Name()+"_tag &rsh);\n}	"+r->Name()+"_t;\n";
		WRITE;
		str="typedef "+r->Name()+"_t	*lp"+r->Name()+"_t;\n\n";
		WRITE;
	}
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
//	These methods generate the parser code.
//	- WriteTerminator writes the termination of a branch when a token of the
//	lexic has been successfully recognized
//	- WriteNode writes the tree structure of the parser : branches and nodes
//	- WriteParser writes the entry point of the parser
//
static map<node*,node*> parserNodes;
void WriteNode(StreamManager& stream,unsigned int file_ID,node *n);

void WriteTerminator(StreamManager& stream,unsigned int file_ID,node *n)
{
	string str;

	if (n == NULL)
		str="	OnError(LEX_ERR_SYN);\n	return -1;\n\n";
	else
	{
		SPRINTF(FORMAT_BUFFER, "	chr = c;\n	return %u;\n\n", n->terminate);
		str = FORMAT_BUFFER;
	}

	WRITE;
}

void WriteBranches(StreamManager& stream,unsigned int file_ID,const vector<branch*> &branches)
{
	for (unsigned int i=0;i<branches.size();i++)
	{
		node *nn = branches[i]->nextNode;

		//	Seek the next node to avoid multiple
		//	node implementations
		map<node*,node*>::const_iterator res = parserNodes.find( nn );

		if (res == parserNodes.end())
		{
			parserNodes[nn] = nn;

			WriteNode(stream,file_ID,nn);
		}
	}
}

void WriteNode(StreamManager& stream,unsigned int file_ID,node *n)
{
	string str,str2;
	node	*terminator=NULL;

	if (n->terminate==0)
	{
		SPRINTF(FORMAT_BUFFER, "%p", n);
		str2 = FORMAT_BUFFER;
		str = "NODE_"+str2+":\n\
	c=GetChar();\n";
		WRITE;

		for (unsigned int i=0;i<n->branches.size();i++)
		{
			branch *b = n->branches[i];

			//	Automat construction ensured that
			//	we only have one terminator per node
			if (b->branchChars.empty())
				terminator = b->nextNode;
			else
			{
				Replace(b->branchChars,"\"","\\\"");
				Replace(b->branchChars,"\\?","");
				SPRINTF(FORMAT_BUFFER, "%p", b->nextNode);
				str2 = FORMAT_BUFFER;

				if (b->branchChars.size()==1)
				{	
					str=string("	if (c=='")+b->branchChars+"')\n\
		goto NODE_"+str2+";\n";
				}
				else
				{
					str=string("	if (NULL != strchr(\"")+b->branchChars+"\",c))\n\
		goto NODE_"+str2+";\n";
				}
				WRITE;
			}
		}
	
		//	each rule should terminate
		WriteTerminator(stream,file_ID,terminator);

		//	Continue parser generation going
		//	deeper in the tree
		WriteBranches(stream,file_ID,n->branches);
	}
}

void WriteCommentHandler(StreamManager& stream,unsigned int file_ID)
{
	string str = "bool CLex::IsComment(unsigned int id)\n{\n";
	if (MicroLexConfig::GetConfig()->RenameClasses())
		Replace(str,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
	WRITE;

	if (comments.size() > 0)
	{
		str = "	bool res = false;\n\n	res = (\n";
		WRITE;

		for (unsigned int i=0;i<comments.size();i++)
		{
			SPRINTF(FORMAT_BUFFER, "			(id == %u)\n", comments[i]);
			str = FORMAT_BUFFER;
			WRITE;
		}

		str = "	      );\n	if(res)\n		OnComment();\n	return res;\n}\n\n";
		WRITE;
	}
	else
	{
		str = "	return false;\n}\n\n";
		WRITE;
	}
}


void WriteParser(StreamManager& stream,unsigned int file_ID)
{
	string str,str2;
	
	str="int CLex::Parse(char &chr)\n\
{\n\
	char c;\n\n\
	c = chr;\n\n";
	if (MicroLexConfig::GetConfig()->RenameClasses())
		Replace(str,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
	WRITE;

	const vector<branch*>& branches = automat.getBranches();

	for (unsigned int i=0;i<branches.size();i++)
	{
		branch *b = branches[i];
		Replace(b->branchChars,"\"","\\\"");
		SPRINTF(FORMAT_BUFFER, "%p", b->nextNode);
		str2 = FORMAT_BUFFER;

		if (b->branchChars.size()==1)
		{	
			str=string("	if (c=='")+b->branchChars+"')\n\
		goto NODE_"+str2+";\n";
		}
		else
		{
			str=string("	if (NULL != strchr(\"")+b->branchChars+"\",c))\n\
		goto NODE_"+str2+";\n";
		}
		WRITE;
	}

	str="	OnError(LEX_ERR_SYN);\n	return -1;\n\n";
	WRITE;

	WriteBranches(stream,file_ID,branches);
	
	str="	chr = c;\n\
}\n\n\
";
	WRITE;
}
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
//	These methods generate the ruler code.
//	- WriteRuler is the main entry point for each rule : it generates
//	sequences completely and a frame for alternates
//	- WriteRuleAlternater generates the code to optimize the call
//	graph of an alternate rule and is called from within WriteRuler
//	- WriteAtomAlternater generates the code to optimize the call
//	graph of an atom list alternate and is called exactly like the previous
//
void WriteRuleAlternater(StreamManager& stream,unsigned int file_ID,rule *r)
{
	string str;
	rule_selector *root = BuildRuleSelectors(r);
	rule_selector *baseRs = root;
	rule_selector *nextBase = (rule_selector *)root->ruleSelectors[0];

	unsigned int depth = 0;

	vector<rule*> completeRules;

	//	The steps of the temporary structures
	//	must be reinited so that parsing
	//	can jump to the appropriate step if
	//	necessary
	for (unsigned int i=0;i<r->GetNbAtoms();i++)
	{
		atom *a = r->GetAtom(i);

		str=string("	status->m_st")+a->Name()+".m_step = 0;\n";
		WRITE;
	}

	while (nextBase != NULL)
	{
		nextBase = NULL;

		//	Push lex state for each depth > 0
		//	because in case of syntax branch fail,
		//	the caller can continue with the correct
		//	sequence of atoms read.
		if (depth==0)
		{
			//	handle the spacial case where the rule
			//	is invoked with a step greater than 0.
			//	Actually, the only other possible is 1
			str = "\n	if(step > 0)\n		result = lex->GetID();\n";
			str+="	else\n		result = lex->ReadAtom();\n\n";
			if (r->AllowEmpty())
				str += "	lex->Push();\n";
		}
		else
		{
			SPRINTF(FORMAT_BUFFER, "STEP_%u:\n	result = lex->ReadAtom();\n	lex->Push();\n", depth);
			str = FORMAT_BUFFER;
		}
		WRITE;
		depth++;

		bool rollback = false;

		for (unsigned int j=0;j<baseRs->ruleSelectors.size();j++)
		{
			rule_selector *curRs = baseRs->ruleSelectors[j];
			if (curRs->ruleSelectors.size()>0)
				nextBase = curRs;

			SPRINTF(FORMAT_BUFFER, "if (result == %s)\n	{\n", IdToString(curRs->selectorID).data());
			str = FORMAT_BUFFER;
			if (j>0) str = "	else "+str; else str = "	" + str;
			WRITE;

			//	handle the case where the rule has the empty flag:
			//	we are in a non empty rule, so, let's pop
			//	the pushed chunck to continue
			//
			//	Also : pop lex state only if it has been pushed
			//	for this rule
			if ((depth > 1) || ((depth == 1) && (r->AllowEmpty())))
			{
				str = "		lex->Pop();\n";
				WRITE;
			}

			unsigned int nb = curRs->selectedRules.size();

			//	If there are more than one rule at this point,
			//	we must generate the code to parse this
			//	rule until it reaches last step and call
			//	each selector callback with the current 
			//	lex chunck
			if (nb>1)
			{
				if (curRs->selector!=NULL)
				{
					str="		m_on"+curRs->selector->Name()+"(result);\n\
		if (abort) return -1;\n";
					WRITE;
					//	here we avoid the cases :
					//	- rule is an alternate with only atoms
					//	- rule is self
					if ((curRs->selector->GetAtom(0)->GetRule() != NULL)
						|| (curRs->selector->IsSequence())) // &&
//						(curRs->selector != r))
					{
						str = "		result=parse"+curRs->selector->Name()+"(1);\n";
						WRITE;
					}
				}
				rule *previous = NULL;
				for (unsigned int i=0;i<curRs->selectedRules.size();i++)
				{
					rule *rr = curRs->selectedRules[i];

					//	Skip duplicate rules. This happens when
					//	- a rule is an alternate
					//	- more than 2 subrules start with the same possible atoms
					if (rr == previous)
						continue;
					previous = rr;

					str="		m_on"+rr->Name()+"(result);\n		if (abort) return -1;\n";
					WRITE;
					//	If rule end here and we are not in the final step
					//	we must call the selector callback because
					//	it will be skipped as other rules are longer.
					//	We can avoid the call to the rule parser because
					//	it will exit imediately with the rule ID
					if ((rr->GetNbAtoms() == depth) && (nextBase != NULL))
					{
						completeRules.push_back(rr);
					}
				}
				if (nextBase != NULL)
				{
					SPRINTF(FORMAT_BUFFER, "		goto STEP_%u;\n	}\n", depth);
					str = FORMAT_BUFFER;
				}
				else
					str = "		goto FINAL_STEP;\n	}\n";
				WRITE;
			}
			//	Otherwise, we are in the case with only one
			//	rule, there is no ambiguity : we parse this rule
			//	and we call both the rule and its parent callbacks
			else
			{
				rule *rr = (rule *)curRs->selectedRules[0];
				atom *a = rr->GetAtom(depth-1);

				//	At this point we have read the first atom. So we
				//	call the subrule with step 1 because here depth >= 1 and
				//	the current rule will jump over it's first element.
				//	This is not generic enough to handle every possible 
				//	case, but the complexity of the language described will
				//	not lead to those possibilities as we stay in LR(1).
				//
				//	Besides, we must call every subrule for the same reason :
				//	each subrule is called with step = 1, so the very first
				//	element is jumped end the subrule state is not updated
				//	with step = 0.
				if (a->GetRule() != NULL)
				{
					vector<string> *array = BuildRulePath(a,a,curRs->selectorID);
					if (NULL != array)
					{
						for (unsigned int i=0;i<array->size();i++)
						{
							string path = array->at(i);
							string p;
							str = "";

							string::size_type pos = path.find('=');

							while (pos != string::npos)
							{
								p = path.substr(0,pos);
								path = path.substr(pos+1);
							
								//  check result: if -1, no rule ID is valid, we can exit from here.
								str = string("		if (-1 == (result=parse")+p+"(1))) return -1;\n" + str;
								str = string("		m_on")+p+"(result);\n" + str;

								pos = path.find('=');
							}

							WRITE;
						}

						delete array;
					}
				}
				
				str="		m_on"+rr->Name()+"(result);\n		if (abort) return -1;\n";
				WRITE;
				SPRINTF(FORMAT_BUFFER, "		result=parse%s(%u);\n", rr->Name().data(), depth);
				str = FORMAT_BUFFER;
				WRITE;
				str="		m_on"+r->Name()+"(result);\n		if (abort) return -1;\n		goto FINAL_STEP;\n	}\n";
				WRITE;
			}

			rollback |= curRs->rollback;
		}

		//	handle lex rollback state
		if (rollback)
			str = "	else\n\
		lex->Rollback();\n";
		else
		{
			//	handle the case where the rule has the empty flag:
			//	we must rollback here so that the next rule can 
			//	use the last atom read, but only on first atom
			str = "	else\n	{\n";
			if (depth > 1)	// depth has already been incremented
			{
				if (completeRules.size()> 0 )
				{
					str += "		result = RULE_"+r->Name()+"_ID;\n";
                    vector<rule*>::const_iterator itr = completeRules.begin();
					while (itr != completeRules.end())
					{
						str += "		m_on"+r->Name()+"(RULE_"+(*itr++)->Name()+"_ID);\n";
					}
                    completeRules.clear();
				}
				str += "		lex->Rollback();\n";
				
			}
			else if (r->AllowEmpty())
			{
				str += "		lex->Rollback();\n		result = RULE_"+r->Name()+"_ID;\n";
			}

			str += "		return result;\n	}\n";
		}
		WRITE;

		baseRs = nextBase;
	}

	str="FINAL_STEP:\n";
	WRITE;

	DeleteRuleSelectors(root);
}


void WriteRuleSequencer(StreamManager& stream,unsigned int file_ID,rule *r)
{
	string str;
	int step = 0;

	for (unsigned int j=0;j<r->GetNbAtoms();j++)
	{
		if (step>0)
		{
			SPRINTF(FORMAT_BUFFER, "STEP_%d:\n	if (step>%d) goto STEP_%d;\n", step, step, step + 1);
			str = FORMAT_BUFFER;
		}
		else
		{
			SPRINTF(FORMAT_BUFFER, "	if (step>%d) goto STEP_%d;\n", step, step + 1);
			str = FORMAT_BUFFER;
		}

		WRITE;
		step++;

		atom *a = r->GetAtom(j);

		if (a->GetRule() != NULL)
		{
			rule *r2 = a->GetRule();
			if (r2->IsLoop())
			{
				if (j<r->GetNbAtoms()-1)
				{
					j++;
					atom *a2 = r->GetAtom(j);
					if (a2->GetRule()==NULL)
					{
						str = string("	PARSE_LOOP_RULE(")+a2->Name()+","+r->Name().data()+","+r2->Name()+");\n";
						WRITE;
					}
					else
					{
						cout << "Cannot parse loop rule " << r->Name() << " followed by another rule !" << endl;
						return;
					}
				}
				else
				{
					cout << endl << "Warning: rule " << r->Name() << " use a loop subrule as last atom. It may not be parsed correctly!" << endl;
					str = string("	PARSE_LOOP_RULE_FINAL(")+r->Name().data()+","+r2->Name()+");\n";
					WRITE;
				}
			}
			else
			{
				if (step > 1)
					str = "	PARSE_SUB_RULE("+a->GetRule()->Name()+","+r->Name()+");\n";
				else
					str = "	PARSE_SUB_RULE0("+a->GetRule()->Name()+","+r->Name()+");\n";
				WRITE;
			}
		}
		else
		{
            if (r->GetAtomOccurences(j) == 1)
            {
			    if (step > 1)
				    str = string("	PARSE_ATOM(")+a->Name()+","+r->Name()+");\n";
			    else
				    str = string("	PARSE_ATOM0(")+a->Name()+","+r->Name()+");\n";
                WRITE;
            }
            else
            {
	            str = "	result = lex->ReadAtom();\n\n";
	            WRITE;

                str = "	lex->Push();\n";
                str+= "	while (result == ATOM_" +a->Name()+"_ID)\n";
                str+= "	{\n		lex->Pop();\n";
                str+= "		m_on"+r->Name()+"(result);\n";
				str+= "		result = lex->ReadAtom();\n";
                str+= "		lex->Push();\n	}\n";
                str+= "	lex->Rollback();\n";
                WRITE;
            }
		}
	}
	SPRINTF(FORMAT_BUFFER, "STEP_%d:\n", step);
	str = FORMAT_BUFFER;
	WRITE;
}

void WriteAtomAlternater(StreamManager& stream,unsigned int file_ID,rule *r)
{
	string str;

	//	handle the spacial case where the rule
	//	is invoked with a step greater than 0.
	//	Actually, the only other possible is 1
	str = "	if(step > 0)\n		result = lex->GetID();\n";
	str+="	else\n		result = lex->ReadAtom();\n\n";
	WRITE;

	if (r->AllowEmpty())
	{
		str="	lex->Push();\n\n";
		WRITE
	}

	str="	if (";
	int nb = r->GetNbAtoms();
	for (int j=nb-1;j>=0;j--)
	{
		atom *a = r->GetAtom(j);

		str+="(result!=ATOM_"+a->Name()+"_ID)";
		if (j>0)
			str+="&&";
		else
			str+=")\n";
	}
	WRITE;

	if (r->AllowEmpty())
	{
		str="	{\n\
		lex->Rollback();\n\
		result = ATOM_empty_ID;\n	}\n	else\n		lex->Pop();\n\n";
		WRITE
	}
	else
	{
		str="		return result;\n";
		WRITE;
	}

	str="	m_on"+r->Name()+"(result);\n	if (abort) return -1;\n";
	WRITE;
}



void WriteRuler(StreamManager& stream,unsigned int file_ID)
{
	string str;

	for (unsigned int i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);
		if (r->IsForward()) continue;

		if (MicroLexConfig::GetConfig()->Verbose())
			cout << "         Building rule " << r->Name() << " function...";
		

		//	Write rule entry point
		str="int CYacc::parse"+r->Name()+"(int step)\n{\n	int result=0;\n";
		if (MicroLexConfig::GetConfig()->RenameClasses())
			Replace(str,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
		WRITE;

		//	Two cases : sequence or alternate.
		//	- first case, the sequence.
		if (r->IsSequence())
		{
			WriteRuleSequencer(stream,file_ID,r);
		}
		//	- second, the case of an alternate
		else
		{
			//	in an alternate, each rule must be of the same
			//	kind : all are atoms or all are rules
			//	So, let's test just the first one
			atom *type = r->GetAtom(0);
			if (type->GetRule() != NULL)
				WriteRuleAlternater(stream,file_ID,r);
			else
				WriteAtomAlternater(stream,file_ID,r);
		}

		//	Handle the case of a forward rule reference:
		//	call back any sub-rule which first sub_rule is this
		if (r->IsForward())
		{
			bool found = false;
			unsigned int pos = 0;
			rule *sub_r = NULL;
			while ((!found) && (pos < r->GetNbAtoms()))
			{
				sub_r = r->GetAtom(pos++)->GetRule();
				if (sub_r != NULL)
				{
					rule *sub_sub_r = sub_r->GetAtom(0)->GetRule();
					found = sub_sub_r == r;
				}
			}

			if (found)
			{
				str = "\n	if (!lex->IsInRollback())\n	{\n\
		lex->Push();\n\
		if (";
				WRITE;
				SPRINTF(FORMAT_BUFFER, "%u", sub_r->ID());
				str = FORMAT_BUFFER;
				str += " != parse" + sub_r->Name() + "(1))\n";
				WRITE;
				str = "			lex->Rollback();\n\
	else\n\
			lex->Pop();\n	}\n";
				WRITE;
			}
		}

		str="\n	result=RULE_"+r->Name()+"_ID;\n\
	return result;\n}\n\n";
		WRITE;

		if (MicroLexConfig::GetConfig()->Verbose())
			cout << " done" << endl;
	}
}
////////////////////////////////////////////////////////////////////////////////






void WriteYaccClass(StreamManager& stream,unsigned int file_ID)
{
	string str;

	str = YACC;

	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		Replace(str,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
		Replace(str,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
	}

	WRITE;

	//	Generate protected member rule parsing functions
	//	User derived classes should overload virtuals
	//	to perform actions during parsing
    unsigned int i=0;
	for (i=0;i<rule::GetNbRules();i++)
	{
		str="		virtual void On"+rule::GetRule(i)->Name()+"(unsigned int) { };\n";
		WRITE;
	}
		
	str="\n	private:\n		bool abort;\n";
	WRITE;

	for (i=0;i<rule::GetNbRules();i++)
	{
		str="		int parse"+rule::GetRule(i)->Name()+"(int step);\n";
		WRITE;
		str="		void m_on"+rule::GetRule(i)->Name()+"(unsigned int RULE_ID);\n";
		WRITE;
	}

	//	Generate type mapping functions
	str="\n\n";
	WRITE;

	unsigned int currentID = 0;
	for (i=0;i<atom::GetNbAtoms();i++)
	{
		atom *a = atom::GetAtom(i);

		//	To avoid re-declaration of a member due to
		//	optional charater ? for atoms, which generates
		//	several rules with the same ID
		if (a->ID()>currentID)
		{
			if (!a->MapType().empty())
			{
                str=string("		virtual ")+a->MapType()+"	MapType"+a->Name()+"(string toMap) const = 0;\n";
				WRITE;
			}
			currentID = a->ID();
			if (currentID == ~0)
				currentID = 0;
		}
	}

	str="};\n\n";
	WRITE;
}


void WriteAnalyser(StreamManager& stream,unsigned int file_ID)
{
	string str;

	str = ANALYSE;
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		Replace(str,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
		Replace(str,"main","main"+MicroLexConfig::GetConfig()->GetYaccClass());
	}
	WRITE;
}


void WriteYaccStatus(StreamManager& stream,unsigned int file_ID)
{
	string str;
	string str2;
	string str3;

	for (unsigned int i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);
		if (r->IsForward()) 
			continue;

		if (r->IsSequence())
		{
			str="void CYacc::m_on"+r->Name()+"(unsigned int RULE_ID)\n\
{\n	"+r->Name()+"_t& st = status->m_st"+r->Name()+";\n	switch(st.m_step)\n	{\n";
			if (MicroLexConfig::GetConfig()->RenameClasses())
				Replace(str,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());

			WRITE;

			atom *a = NULL;
			unsigned int max = r->GetNbAtoms();

			for (unsigned int j=0;j<max;j++)
			{
				a = r->GetAtom(j);

				if (!a->MapType().empty())
				{
					SPRINTF(FORMAT_BUFFER, "%u", j);
					str2 = FORMAT_BUFFER;

					
					if (r->GetAtomOccurences(j) > 1)
					{
						str="		case 0:\n";
						WRITE;
						str=string("			st.st_")+a->Name()+str2+".clear();\n";
						WRITE;
						str = "			break;\n";
						WRITE;
						str="		case "+str2+":\n";
						WRITE;
						str=string("			st.st_")+a->Name()+str2+"\
.push_back(MapType"+a->Name()+"(lex->GetChunck()));\n";
						WRITE;
					}
					else
					{
						str="		case "+str2+":\n";
						WRITE;
						str=string("			st.st_")+a->Name()+str2+" \
= MapType"+a->Name()+"(lex->GetChunck());\n";
						WRITE;
					}

					if (j == max-1)
					{
						if (r->GetAtomOccurences(j) > 1)
							str="			st.m_step = " + str2 + " - 1;\n";
						else
							str="			st.m_step = -1;\n";
						WRITE;
					}

					str = "			break;\n";
					WRITE;
				}
				else if (a->GetRule()!=NULL)
				{
					SPRINTF(FORMAT_BUFFER, "%u", j);
					str2 = FORMAT_BUFFER;

					str="		case "+str2+":\n";
					WRITE;

					rule *rr = a->GetRule();

					if (!rr->IsLoop())
						str=string("			st.st_")+a->Name()+str2+" \
= status->m_st"+a->Name()+";\n";
					else
						str=string("			st.st_")+a->Name()+str2+"\
.push_back(status->m_st"+a->Name()+");\n";
					WRITE;

					if (j == max-1)
					{
						str="			st.m_step=-1;\n";
						WRITE;
					}

					str = "			break;\n";
					WRITE;
				}
				else if (j == max-1)
				{
					SPRINTF(FORMAT_BUFFER, "%u", j);
					str2 = FORMAT_BUFFER;

					str="		case "+str2+":\n\
			st.m_step = -1;\n			break;\n";
					WRITE;
				}
			}

			SPRINTF(FORMAT_BUFFER, "%u", max - 1);
			str2 = FORMAT_BUFFER;
			str="		default:\n\
			if (st.m_step > " + str2 + ")\n\
				OnError(CYacc::YACC_ERR_SYN);\n\
			break;\n	}\n\n	st.m_step++;\n";
			if (MicroLexConfig::GetConfig()->RenameClasses())
				Replace(str,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
			WRITE;
		}
		//	The rule is an alternate
		else
		{
			str="void CYacc::m_on"+r->Name()+"(unsigned int RULE_ID)\n\
{\n	"+r->Name()+"_t& st = status->m_st"+r->Name()+";\n\
	st.st_"+r->Name()+"ID = RULE_ID;\n	st.m_step = 0;\n";
			if (MicroLexConfig::GetConfig()->RenameClasses())
				Replace(str,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
		
			WRITE;

			atom *a = r->GetAtom(0);
			if (a->GetRule() != NULL)
			{
				str ="\n	switch(RULE_ID)\n	{\n";
				WRITE;

				for (unsigned int k=0;k<r->GetNbAtoms();k++)
				{
					a = r->GetAtom(k);
					SPRINTF(FORMAT_BUFFER, "%u", a->GetRule()->ID());
					str2 = FORMAT_BUFFER;
					str="		case "+str2+":\n		{\n\
			"+a->GetRule()->Name()+"_t *p = new " + a->GetRule()->Name()+"_t;\n";
					WRITE;
					str = string("			*p = status->m_st")+a->GetRule()->Name()+";\n";
					WRITE;
					str = string("			st.p_st")+r->Name()+" = p;\n			break;\n		}\n";
					WRITE;
				}

				str="		default:\n			st.p_st"+r->Name()+" = NULL;\n\
			break;\n	}\n\n";
				WRITE;
			}
			else if (!a->MapType().empty())
			{
				str = "	switch (RULE_ID)\n	{\n";
				WRITE;
				string id;
				for (unsigned int k=0;k<r->GetNbAtoms();k++)
				{
					a = r->GetAtom(k);
					SPRINTF(FORMAT_BUFFER, "%s_value.%s_%u", r->Name().data(), a->Name().data(), k);
					id = FORMAT_BUFFER;
					str = string("		case ATOM_")+a->Name()+"_ID:\n			st."+
						id + " = MapType"+a->Name()+"(lex->GetChunck());\n			break;\n";
					WRITE;
				}
				str = "	}\n";
				WRITE;
			}
		}
		
		str="	On"+r->Name()+"(RULE_ID);\n}\n\n";
		WRITE;
	}
}



////////////////////////////////////////////////////////////////////////////////
//
//	Generation of the class that stores the current atoms and rules parsed.
//	This class represents the parse tree and it is member of CYacc
//
//
void WriteStatusClass(StreamManager& stream,unsigned int file_ID)
{
	string str="class CYaccStatus\n\
{\n\
	public:\n\
		CYaccStatus() {};\n\
		~CYaccStatus() {};\n\n\
		void	Init(void);\n\n\
	public:\n";
	if (MicroLexConfig::GetConfig()->RenameClasses())
		Replace(str,"CYaccStatus",MicroLexConfig::GetConfig()->GetYaccClass()+"Status");

	WRITE;

	unsigned int currentID = 0;
    unsigned int i=0;
	for (i=0;i<atom::GetNbAtoms();i++)
	{
		atom *a = atom::GetAtom(i);

		//	To avoid re-declaration of a member due to
		//	optional charater ? for atoms, which generates
		//	several rules with the same ID
		if (a->ID()>currentID)
		{
			if (!a->MapType().empty())
			{
				str=string("		")+a->MapType()+"		m_"+a->Name()+";\n";
				WRITE;
			}

			currentID = a->ID();
			if (currentID == ~0)
				currentID = 0;
		}
	}

	str="\n";
	WRITE;

	for (i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);

		str="		"+r->Name()+"_t	m_st"+r->Name()+";\n";
		WRITE;
	}

	str="};\n\n";
	WRITE;
}

//
//	Only one method : an initialiser
//
void WriteStatusCode(StreamManager& stream,unsigned int file_ID)
{
	string str="void CYaccStatus::Init(void)\n{\n";

	if (MicroLexConfig::GetConfig()->RenameClasses())
		Replace(str,"CYaccStatus",MicroLexConfig::GetConfig()->GetYaccClass()+"Status");

	WRITE;
	
	//	Clear arrays
	for (unsigned int i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);

		if (r->IsSequence())
		{
			for (unsigned int j=0;j<r->GetNbAtoms();j++)
			{
				atom *a = r->GetAtom(j);
				
				if (a->GetRule() != NULL)
				{
					rule *rr = a->GetRule();
				
					if (rr->IsLoop())
					{
						ostrstream id;
						id << j << ends;

						str = "	m_st"+r->Name();
						str += string(".st_")+rr->Name()+id.str()+".clear();\n";
						WRITE;
					}
				}
			}
		}

		str = "	m_st"+r->Name()+".m_step = 0;\n";
		WRITE;
	}
	

	str="}\n\n";
	WRITE;
}

//
//	This method writes the operator= members, as they are
//	the way the parse tree is built step by step
//
void WriteOperators(StreamManager& stream,unsigned int file_ID)
{
	string str;

	str="\n\n//Syntaxic structures operators\n";
	WRITE;

	for (unsigned int i=0;i<rule::GetNbRules();i++)
	{
		rule *r = rule::GetRule(i);
		if (r->IsForward()) continue;

		str=r->Name()+"_tag& "+r->Name()+"_tag::operator=(const "+r->Name()+"_tag& rsh)\n{\n";
		WRITE;

		if (r->IsSequence())
		{
			for (unsigned int j=0;j<r->GetNbAtoms();j++)
			{
				ostrstream id;
				atom *a = r->GetAtom(j);
				if (!a->MapType().empty())
				{
					id << j << ends;
					str=string("	st_")+a->Name()+id.str()+" = rsh.st_"+a->Name()+id.str()+";\n";
					WRITE;
				}
				else if (a->GetRule() != NULL)
				{
					rule *rr = a->GetRule();
					id << j << ends;
					const string &n = rr->Name();

					if (rr->IsLoop())
					{
						str="	st_"+n+id.str()+" = rsh.st_"+n+id.str()+";\n";
						WRITE;
					}
					else
					{
						str="	st_"+n+id.str()+" = rsh.st_"+n+id.str()+";\n";
						WRITE;
					}
				}
			}
		}
		else
		{
			str="	st_"+r->Name()+"ID = rsh.st_"+r->Name()+"ID;\n";
			WRITE;
			atom *aa = r->GetAtom(0);
			if (aa->GetRule() != NULL)
			{
				str="	p_st"+r->Name()+" = rsh.p_st"+r->Name()+";\n";
				WRITE;
			}
			else if (!aa->MapType().empty())
			{
				string id;
				for (unsigned int k=0;k<r->GetNbAtoms();k++)
				{
					aa = r->GetAtom(k);
					SPRINTF(FORMAT_BUFFER, "%s_value.%s_%u", r->Name().data(), aa->Name().data(), k);
					id = FORMAT_BUFFER;
					str = "	" + id + " = rsh." + id + ";\n";
					WRITE;
				}
			}
		}

		//	structure step & assignment operator
		str="	m_step = rsh.m_step;\n	return *this;\n}\n\n";
		WRITE;
	}
}
////////////////////////////////////////////////////////////////////////////////









////////////////////////////////////////////////////////////////////////////////
//
//	Main functions : generation of headers and sources
//
//
//
void WriteHeader(StreamManager& stream,unsigned int filelex,unsigned int fileyacc,unsigned int filetree)
{
	bool verbose = MicroLexConfig::GetConfig()->Verbose();

	if (verbose)
		cout << "      Generating interface header ...";
	
	stream.WriteData(filelex,HEADER);
	stream.WriteData(fileyacc,HEADER);
	stream.WriteData(filetree,HEADER);

	string head = "#if !defined(__"+MicroLexConfig::GetConfig()->GetLexClass()+"LEX_H__)\n\
#define __"+MicroLexConfig::GetConfig()->GetLexClass()+"LEX_H__\n\n";
	stream.WriteData(filelex,head.data());

	head = "#if !defined(__"+MicroLexConfig::GetConfig()->GetYaccClass()+"TREE_H__)\n\
#define __"+MicroLexConfig::GetConfig()->GetYaccClass()+"TREE_H__\n\n";
	stream.WriteData(filetree,head.data());

	head = "#if !defined(__"+MicroLexConfig::GetConfig()->GetYaccClass()+"YACC_H__)\n\
#define __"+MicroLexConfig::GetConfig()->GetYaccClass()+"YACC_H__\n\n\
class " + MicroLexConfig::GetConfig()->GetLexClass() + ";";
	stream.WriteData(fileyacc,head.data());
	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating interface defines ...";
	WriteRuleDefines(stream,filelex);
	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating structure types ...";
	WriteTypes(stream,filetree);
	if (verbose)
		cout << " done." << endl;

	head = "\n\nclass CYaccStatus;\n\n";
	if (MicroLexConfig::GetConfig()->RenameClasses())
		Replace(head,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
	stream.WriteData(fileyacc,head.data());
	if (verbose)
		cout << "      Generating interface classes...";
	WriteStatusClass(stream,filetree);

	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		head = LEXCLASS;
		Replace(head,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		stream.WriteData(filelex,head.data());
	}
	else
		stream.WriteData(filelex,LEXCLASS);
	WriteYaccClass(stream,fileyacc);
	if (verbose)
		cout << " done." << endl;

	head = "#endif\n";
	stream.WriteData(filelex,head.data());
	stream.WriteData(filetree,head.data());
	stream.WriteData(fileyacc,head.data());
}
//
//
void WriteCode(StreamManager& stream,unsigned int filelex,unsigned int fileyacc,unsigned int filetree)
{
	bool verbose = MicroLexConfig::GetConfig()->Verbose();

	if (verbose)
		cout << "      Generating code header..." << endl;

	string fname;
	stream.WriteData(filelex,HEADER);
	stream.WriteData(fileyacc,HEADER);
	stream.WriteData(filetree,HEADER);

	fname = stream.GetFileName(filelex);
	fname = fname.substr(0,fname.size()-3);
	fname = "#include \""+fname+"h\"\n";
	stream.WriteData(filelex,fname.data());
	stream.WriteData(fileyacc,fname.data());

	fname = stream.GetFileName(fileyacc);
	fname = fname.substr(0,fname.size()-3);
	fname = "#include \""+fname+"h\"\n";
	stream.WriteData(fileyacc,fname.data());

	fname = stream.GetFileName(filetree);
	fname = fname.substr(0,fname.size()-3);
	fname = "#include \""+fname+"h\"\n";
	stream.WriteData(filetree,fname.data());
	stream.WriteData(fileyacc,fname.data());

	stream.WriteData(fileyacc,DEFINE);

	fname = "\n\n#include <string.h>\n\n\
CLex::CLex():bufferpos(BUFFER_SIZE),maxpos(BUFFER_SIZE),sourceReady(false),buffer(NULL)\n\
{}\n\n\
CLex::~CLex()\n\
{\n\
	if (buffer != NULL)\n\
		delete [] buffer;\n\
}\n\n\
";
	if (MicroLexConfig::GetConfig()->RenameClasses())
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
	stream.WriteData(filelex,fname.data());
	WriteCommentHandler(stream,filelex);

	fname = "\n\n\n\
CYacc::CYacc(CLex *parser):lex(parser),abort(false)\n\
{\n	status = new CYaccStatus();\n}\n\n\
CYacc::~CYacc()\n\
{\n	delete status;\n}\n\n\
";
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		Replace(fname,"CYacc",MicroLexConfig::GetConfig()->GetYaccClass());
	}
	stream.WriteData(fileyacc,fname.data());

	if (MicroLexConfig::GetConfig()->Verbose())
		cout << "      Generating io-buffer code ...";
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		fname = BUFFER;
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		stream.WriteData(filelex,fname.data());
	}
	else
		stream.WriteData(filelex,BUFFER);

	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating analyser init/terminate code ...";
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		fname = INIT;
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		stream.WriteData(filelex,fname.data());
	}
	else
		stream.WriteData(filelex,INIT);
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		fname = FINISH;
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		stream.WriteData(filelex,fname.data());
	}
	else
		stream.WriteData(filelex,FINISH);
	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating prefetch code ...";
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		fname = PREFETCH;
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		stream.WriteData(filelex,fname.data());
	}
	else
		stream.WriteData(filelex,PREFETCH);
	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating analyser status code ...";
	WriteOperators(stream,filetree);
	WriteStatusCode(stream,filetree);
	WriteYaccStatus(stream,fileyacc);
	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating parser code ...";
	WriteParser(stream,filelex);
	if (MicroLexConfig::GetConfig()->RenameClasses())
	{
		fname = READATOM;
		Replace(fname,"CLex",MicroLexConfig::GetConfig()->GetLexClass());
		stream.WriteData(filelex,fname.data());
	}
	else
		stream.WriteData(filelex,READATOM);
	if (verbose)
		cout << " done." << endl;

	if (verbose)
		cout << "      Generating analyser code ..." << endl;
	WriteRuler(stream,fileyacc);
	WriteAnalyser(stream,fileyacc);
	if (verbose)
		cout << "      done." << endl;
}
////////////////////////////////////////////////////////////////////////////////
