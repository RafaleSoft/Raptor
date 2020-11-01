/***************************************************************************/
/*                                                                         */
/*  Rules.cpp                                                              */
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

#include <stdio.h>	// EOF,...
#include <string.h>	// memset,...

extern vector<unsigned int>	comments;

using namespace std;

//	new ID given when new rule is created
//	and that will be used to find a unique rule
unsigned int ATOM_ID = 1;

vector<rule*> rule::m_rules;

rule::rule(const string &name,unsigned int id,bool sequence,bool forward)
{
	m_name = name;
	m_ruleID = id;
	m_forward = forward;
	m_loop = false;
	m_isSequence = sequence;
	m_allowEmpty = false;
}

rule::~rule()
{
}

atom* rule::GetAtom(unsigned int pos)
{
	if (pos >= m_atomList.size())
		return NULL;
	else
		return m_atomList[pos].pAtom;
}

void rule::SetAtomOccurences(unsigned int pos,int occurences)
{
    if (pos < m_atomList.size())
    {
        int occ = occurences;
        if (occ < 1)
            occ = 1;
        m_atomList[pos].occurences = occ;
    }
}

int rule::GetAtomOccurences(unsigned int pos)
{
    if (pos >= m_atomList.size())
		return -1;
	else
		return (m_atomList[pos].occurences);
}

void rule::AddAtom(atom* a)
{
	if (a != NULL)
    {
        atom_occurence occ;
        occ.pAtom = a;
        occ.occurences = 1;

		m_atomList.push_back(occ);
    }
}

bool rule::RemoveAtom(unsigned int pos)
{
	if (pos >= m_atomList.size())
		return false;
	else
	{
		m_atomList.erase(m_atomList.begin() + pos);
		return true;
	}
}

rule* rule::GetRule(unsigned int pos)
{
	if (pos >= m_rules.size())
		return NULL;
	else
		return m_rules[pos];
}

rule* rule::FindRule(const string& name)
{
	bool found=false;
	unsigned int i=0;

	while ((i<m_rules.size()) && (!found))
		found = (name == m_rules[i++]->Name());

	if (found)
		return m_rules[i-1];
	else
		return NULL;
}

void rule::DestroyRules(void)
{
	if (m_rules.size()>0)
	{
		for (unsigned int i=0;i<m_rules.size();i++)
		{
			rule *r = m_rules[i];

//				for (int j=0;j<r->atomList.GetSize();j++)
//				{
//					atom *a = (atom *)(r->atomList[j]);
//					if (a->GetRule() != NULL)
//						delete a;
//				}
			delete r;
		}
		m_rules.clear();
	}
}


//	Display rule content
void rule::Display(void)
{
	cout << "RULE parsed:" << endl;
	cout << "   rule: " << m_name << endl;
	cout << "  atoms: ";
	
	size_t i = 0;
	size_t nb = m_atomList.size();

	if (nb > 0)
	{
		for (i=0; i < nb-1; i++)
        {
			cout << (m_atomList[i].pAtom)->Name();
            if (m_atomList[i].occurences > 1)
                cout << "(" << m_atomList[i].occurences << ")";
            cout << ",";
        }
	
		cout << (m_atomList[i].pAtom)->Name();
        if (m_atomList[i].occurences > 1)
            cout << "(" << m_atomList[i].occurences << ")";
        cout << endl;
	}

	cout << endl;
}

//	Add a new rule to the set,
//	replacing a forward definition if so,
//	producing an error otherwise
int rule::AddRule(rule *r)
{
	rule *rr = FindRule(r->Name());
	if (rr != NULL)
	{
		if (rr->IsForward())
		{
			rr->Forward(false);
			rr->Sequence(r->IsSequence());
			rr->Loop(r->IsLoop());

			for (unsigned int i=0;i<r->m_atomList.size();i++)
				rr->m_atomList.push_back(r->m_atomList[i]);
		}
		else
		{
			cout << "Rule '" << r->Name() << "' is already defined and is not a forward declaration." << endl;
			return -1;
		}
	}
	else
	{
		m_rules.push_back(r);
	}

	return 0;
}


string IdToString(unsigned int id)
{
	string res = "";
	bool found = false;
	unsigned int pos = 0;

	// search atoms
	while ((!found) && (pos < atom::GetNbAtoms()))
	{
		atom *a = atom::GetAtom(pos);
		found = (a->ID() == id);
		if (found)
			res = "ATOM_"+a->Name()+"_ID";
		else
			pos++;
	}

	// search rules
	pos = 0;
	while ((!found) && (pos < rule::GetNbRules()))
	{
		rule *r = rule::GetRule(pos);
		found = (r->ID() == id);
		if (found)
			res = "RULE_"+r->Name()+"_ID";
		else
			pos++;
	}

	return res;
}


//	build a rule defined by
//	rule=[...]
char ReadChoice(StreamManager& stream,atom *r)
{
	string valid="";
	char c = stream.ReadToken(']',valid);

	// Handle the 'everything but' capability
	if ((valid[0] == '^') && (valid.size() > 1))
	{
		bool useChars[128];
		memset(useChars,0,128*sizeof(bool));
		unsigned int pos = 1;
		while (pos < valid.size())
		{
			// caracter '\$' expands to EOL
			if (valid[pos] == '\\')
			{
				if ((pos < valid.size() - 1) && (valid[pos+1] == '$'))
				{
					pos++;
					valid[pos] = 0x04;
				}
			}
			useChars[valid[pos++]] = true;
		}

		valid.erase();
		//	start at 'keyboard' chars ...
		for (pos=32;pos<128;pos++)
		{
			if (!useChars[pos])
				valid += (char)(pos);
			if (pos == 92)	//	Add a '\\'
				valid += (char)(pos);
		}
		//	... and add the tab
		if (!useChars[9])
			valid += (char)(9);
	}

	r->AddValidChars(valid.data());
	return c;
}

//	build an atom defined by
//	atom=<...>, i.e. another atom
char ReadAtomRef(StreamManager& stream,atom *a,rule *r)
{
	string ruleref="";
	char c = stream.ReadToken('>',ruleref);

	// search rule reference
	atom *aa = NULL;

	if (ruleref == "empty")
	{
		r->AllowEmpty(true);
		return c;
	}
	else 
		aa = atom::FindAtom(ruleref.data());

	if (aa != NULL)
	{
		if (a!=NULL)
		{
			for(unsigned int j=0;j<aa->NbValidChars();j++)
				a->AddValidChars(aa->GetValidChars(j));
		}
		else if (r!=NULL)
			r->AddAtom(aa);
	}
	else
	{
		cout << "Unknown rule reference: '" << ruleref.data() << "'" << endl;
		c = EOF;
	}

	return c;
}

//	build a rule defined by
//	an identifier or a sequence of chars
char ReadAtomDef(StreamManager& stream,atom *r,char c_read)
{
	char c = c_read;
	string c_rule;

	while ((c != EOF)&&(!IS_SEPARATOR(c)))
	{
		//	cut backslashes
		if (c != '\\')
		{
			c_rule = c;
			r->AddValidChars(c_rule);
		}
		c = stream.GetChar();

	}
	return c;
}



//	Parses an existing atom/rule name and
//	associate it to a well defined C++ type
//	( token after '=' )
int MapToType(StreamManager& stream,char &car)
{
	string atomName;
	string type;

	// read token
	char c = stream.ReadToken('=',atomName);
	atom *a = atom::FindAtom(atomName.data());

	if (a == NULL)
	{
		cout << "Atom " << atomName.data() << " is not defined" << endl;
		car = c;
		return -1;
	}

	c = stream.ReadToken(EOF,type);

	//	Display and add typedef to atom
	if (MicroLexConfig::GetConfig()->Verbose())
	{
		cout	<< "Token " 
				<< atomName.data() 
				<< " mapped to C++ type " 
				<< type.data() 
				<< endl << endl;
	}
	a->MapType(type.data());

	//	current caracter must be returned
	//	to continue parent line processing
	car=c;
	return 0;
}

//	Define an atom to be a comment.
//	Comments are not returned by the lexer.
//	User can handle them in the lexer comment callback
int MapToComment(StreamManager& stream,char &car)
{
	string atomName;
	char c = stream.ReadToken(EOF,atomName);

	atom *a = atom::FindAtom(atomName.data());
	if (a == NULL)
	{
		cout << "Atom " << atomName.data() << " is not defined" << endl;
		car = c;
		return -1;
	}
	else
		comments.push_back(a->ID());
	
	//	Display and add typedef to atom
	if (MicroLexConfig::GetConfig()->Verbose())
	{
		cout	<< "Token " 
				<< atomName.data() 
				<< " mapped to comment " 
				<< endl << endl;
	}

	//	current caracter must be returned
	//	to continue parent line processing
	car=c;
	return 0;
}



//	Parses the definitionsof an atom:
//	atom:<name>=[<any chars>|['['<any chars>']'[+*]*]*]
int ParseAtom(StreamManager& stream,char &car)
{
	// read identifier
	string atomName;
	char c = stream.ReadToken('=',atomName);
	if (c == EOF)
		return -1;

	atom *a = new atom(atomName.data(),ATOM_ID++);

	if (a->Name() == "empty")
	{
		cout << "Error: cannot use built-in keyword 'empty' to define an atom" << endl;
		return -1;
	}

	// read definition
	c = stream.GetChar();
	size_t l = 0;
	size_t l2 = 0;
	while ((!IS_SEPARATOR(c))&&(c != EOF))
	{
		switch (c)
		{
			case '[':
				l = a->NbValidChars();
				if (']' != ReadChoice(stream,a))
				{
					cout << "Syntax error: unable to reach matching ']'" << endl;
					return 100;
				}
				break;
			case '<':
				l = a->NbValidChars();
				if ('>' != ReadAtomRef(stream,a,NULL))
				{
					cout << "Syntax error: unable to reach matching '>'" << endl;
					return 100;
				}
				break;
			case '+':
				l2 = a->NbValidChars();
				if (l2 > l + 1)
				{
					cout << "Syntax error: cannot apply dup on a sequence of rules" << endl;
					return 100;
				}
				else
				{
					//	transform a+ -> aa*
					string s = "\\*" + a->GetValidChars(l);
					a->AddValidChars(s);
				}
				break;
			case '*':
				l2 = a->NbValidChars();
				if (l2 > l + 1)
				{
					cout << "Syntax error: cannot apply dup on a sequence of rules" << endl;
					return 100;
				}
				else
				{
					string s = "\\*" + a->GetValidChars(l);
					a->SetValidChars(l,s);
				}
				break;
			case '?':
				l2 = a->NbValidChars();
				if (l2 > l + 1)
				{
					cout << "Syntax error: cannot apply optional on a sequence of rules" << endl;
					return 100;
				}
				else
				{
					string s = "\\?" + a->GetValidChars(l);
					a->SetValidChars(l,s);
				}
				break;
			default:
				c = ReadAtomDef(stream,a,c);
				break;
		}

		c = stream.GetChar();
	}

	//	Display and add rule to repository
	if (a->NbValidChars() == 0)
	{
		cout << "Syntax error: atom definition is empty !" << endl;
		return 100;
	}

	if (MicroLexConfig::GetConfig()->Verbose())
		a->Display();

	//	current caracter must be returned
	//	to continue parent line processing
	car = c;

	if (!atom::AddAtom(a))
	{
		cout << "Atom '" << a->Name() <<"' already defined, aborting." << endl;
		return -1;
	}
	else
		return 0;
}


//	Search a rule in the set
//	of all already parsed rules.
//	This allows to define a rule
//	using other rules
char ReadRuleRef(StreamManager& stream,rule *r)
{
	string ruleref="";
	char c = stream.ReadToken('}',ruleref);

	rule *r2 = rule::FindRule(ruleref);

	if (r2 != NULL)
	{
		atom *a = new atom(r2->Name().data(),0,r2);
		r->AddAtom(a);
	}
	else
	{
		if (ruleref.data() == r->Name())
		{
			if (r->IsLoop())
			{
				cout << "Rule loop not unique: '" << ruleref.data() << "'" << endl;
				c = EOF;
			}
			else
			{
				r->Loop(true);
				atom *a = new atom(r->Name().data(),0,r);
				r->AddAtom(a);
			}
		}
		else
		{
			cout << "Unknown rule reference: '" << ruleref.data() << "'" << endl;
			c = EOF;
		}
	}

	return c;
}


//	Parses a forward declaration of a rule.
//	This allows to define loops beetween rules.
//	Caution! : it is user responsibility to check
//	cycles
int ParsePredecl(StreamManager& stream,char &c)
{
	// read identifier
	string ruleName;
	char car = stream.ReadToken('\n',ruleName);

	rule *r = new rule(ruleName,ATOM_ID++,false,true);

	//	Display and add rule to repository
	if (MicroLexConfig::GetConfig()->Verbose())
		r->Display();

	c = car;
	return rule::AddRule(r);
}

int ParseRule(StreamManager& stream,char &car,bool isSequence)
{
	// read identifier
	string ruleName;
	char c = stream.ReadToken('=',ruleName);

	//	Retrieve a predefined rule ID, or a new ID otherwise
	unsigned int newID = 0;
	rule *r = rule::FindRule(ruleName);
	if (r != NULL)
		newID = r->ID();
	else
		newID = ATOM_ID++;
	r = new rule(ruleName,newID,isSequence,false);

	// read definition
	c=stream.GetChar();
	while ((!IS_SEPARATOR(c))&&(c != EOF))
	{
		switch (c)
		{
			case '<':
				if ('>' != ReadAtomRef(stream,NULL,r))
				{
					cout << "Syntax error: unable to reach matching '>'" << endl;
					cout << "Aborting parsing in rule: " << r->Name() << endl;
					return 100;
				}
				break;
			case '{':
				if ('}' != ReadRuleRef(stream,r))
				{
					cout << "Syntax error: unable to reach matching '}'" << endl;
					cout << "Aborting parsing in rule: " << r->Name() << endl;
					return 100;
				}
				break;
            case '*':
                if (r->GetNbAtoms() > 0)
                {
                    atom *a = r->GetAtom(r->GetNbAtoms()-1);
                    if (a->GetRule() != NULL)
                    {
                        cout << "Syntax error: multiple occurences marker is only supported on atoms" << endl;
					    cout << "Aborting parsing in rule: " << r->Name() << endl;
					    return 100;
                    }
                    else
                        r->SetAtomOccurences(r->GetNbAtoms() - 1,2);
                }
                else
                {
                    cout << "Syntax error: no initial atom for multiple occurences marker" << endl;
					cout << "Aborting parsing in rule: " << r->Name() << endl;
					return 100;
                }
                break;
			default:
				cout << "Syntax error: invalid character '" << c << "'" << endl;
				cout << "Rule declaration must use <> for atoms and {} for rules" << endl;
				cout << "Aborting parsing in rule: " << r->Name() << endl;
				return 100;
				break;
		}
		c=stream.GetChar();
	}

	if (r->GetNbAtoms() == 0)
	{
		cout << "Syntax error: separator found in rule definition or rule is empty !" << endl;
		return 100;
	}

	//	rule validation for a loop :
	//	loop must be unique and placed at end
	if (r->IsLoop())
	{
		int size = r->GetNbAtoms();
		atom *a = r->GetAtom(size-1);
		if (a->GetRule() == NULL)
		{
			cout << r->Name() << " rule loop is incorrect" << endl;
			return -1;
		}
		else if (r->Name() != a->GetRule()->Name())
		{
			cout << r->Name() << " rule loop is incorrect" << endl;
			return -1;
		}
		else
		{
			// rule is valid
		//	delete a;
			r->RemoveAtom(size-1);
		}
	}
	//	rule validation for alternates:
	//	all items of the alternate must be of the same kind
	//	if items are rules, they must not be loops
	//	if empty is specified, alternate can only be atoms
	else if (!r->IsSequence())
	{
		bool allAtoms = false;
		bool allRules = false;

		for (unsigned int i=0;i<r->GetNbAtoms();i++)
		{
			atom *a = r->GetAtom(i);
			
			if (a->GetRule() != NULL)
			{
				if (allAtoms)
				{
					cout << "All atoms of rule '" << r->Name() << "' must be of the same kind: rules" << endl;
					return -1;
				}
				else if (a->GetRule()->IsLoop())
				{
					cout << "Rule alternate '" << r->Name() << "' cannot contain loops" << endl;
					return -1;
				}
				else
					allRules = true;
			}
			else
			{
				if (allRules)
				{
					cout << "All atoms of rule '" << r->Name() << "' must be of the same kind: atoms" << endl;
					return -1;
				}
				else
					allAtoms = true;
			}
		}
	}
	else
	{
		if (r->AllowEmpty())
		{
			cout << "A sequence rule cannot contain the empty tag" << endl;
			return -1;
		}

		// If rule is a sequence, and if the last atom
		// is a rule, it cannot be an alternate with the
		// empty tag. This functionnality is not supported yet
		// because useless complexity.
		// It is always possible to declare another rule with
		// the same beginning atoms, but with a non empty final
		// alternate rule. The the parser 'factorize' the parsing
		// of the two rules and makes a selection when it reaches
		// the different tail.
		// ex:
		//	this case is not supported:
		//		alternate:subrule=<an atom><empty>
		//		sequence:toto=<another atom>{subrule}
		//	it can be rewritten like this:
		//		alternate:subrule=<an atom>
		//		sequence:toto=<atom1>
		//		sequence:toto2=<atom1>{subrule}
		int sz = r->GetNbAtoms();
		atom *a = r->GetAtom(sz-1);

		if (a->GetRule() != NULL)
		{
			if (a->GetRule()->AllowEmpty())
			{
				cout << "A sequence rule cannot contain a terminal empty rule (";
				cout << r->Name() << " contains " << a->Name() << ")" << endl;
				return -1;
			}
		}
	}

	//	Display and add rule to repository
	if (MicroLexConfig::GetConfig()->Verbose())
		r->Display();

	//	current caracter must be returned
	//	to continue parent line processing
	car=c;
	
	return rule::AddRule(r);
}


//	This method builds the rule path starting from
//	the given atom. The paths are strings returned in
//	the array, each string is a path, build with
//	rule names separated by '='. This method searches
//	all paths that lead to id. A correct language description
//	should return only one path.
//	If no paths are found, the returned array is NULL and
//	should be handled as an error
vector<string> *BuildRulePath(atom* start, atom* a,unsigned int id)
{
	vector<string> *paths = NULL;

	if (a->GetRule() != NULL)
	{
		rule *r = a->GetRule();
		
		size_t max = 1;
		if (!r->IsSequence())
			max = r->GetNbAtoms();

		for (size_t j=0; j < max; j++)
		{
			atom *aa = r->GetAtom(j);
			vector<string> *p = NULL;
			
			if (aa != start)
				p = BuildRulePath(start, aa, id);

			if (p != NULL)
			{
				if (paths == NULL)
					paths = new vector<string>;
				for (size_t i=0; i < p->size(); i++)
					paths->push_back(string(a->Name()+"="+p->at(i)));
				delete p;
			}
		}
	}
	else
	{
		if (a->ID() == id)
		{
			paths = new vector<string>;
			paths->push_back(a->Name());
		}
	}

	return paths;
}


rule *DeepAtomSearch(atom *macroAtom)
{
	rule *ret = new rule("",0,false,false);
	rule *atomList = NULL;

	rule *r = macroAtom->GetRule();
	int nb = r->GetNbAtoms();
	atom *aa = NULL;

	for (int i=0;i<nb;i++)
	{
		aa = r->GetAtom(i);

		//	We need a variant array to store rule nodes:
		//	Searching all deepest atoms can go through
		//	an alternate of rules, and we must explore
		//	each branch. We avoid recursion doing this.
		vector<atom*> aas;
		aas.push_back(aa);

		while (aas.size() > 0)
		{
			aa = aas.back();
			aas.pop_back();

			// Avoid language recursion : an atom of a subrule
			// can be equal to the entry point atom. As we are 
			// searching deepest atoms of the entry rule, we can
			// stop recursion if we find the same atom as the first
			// member of any subrule.
			while ((aa->GetRule() != NULL) && (aa != macroAtom))
			{
				//	Are we in the case where the rule is
				//	an alternate and all atoms are simple atoms ?
				if (aa->GetRule()->IsSequence() == false)
				{
					atomList = aa->GetRule();
					for (unsigned int k=1;k<atomList->GetNbAtoms();k++)
						if (atomList->GetAtom(k)->GetRule() != NULL)
							aas.push_back(atomList->GetAtom(k));
				}
				else
					atomList = NULL;
				aa = aa->GetRule()->GetAtom(0);
			}

			if (atomList != NULL)
			{
				for (unsigned int j=0;j<atomList->GetNbAtoms();j++)
					ret->AddAtom(atomList->GetAtom(j));
				atomList = NULL;
			}
			else if (aa != macroAtom)
				ret->AddAtom(aa);
		}
	}
	
	return ret;
}

void BuildRuleSelector(rule_selector *r,unsigned int selectorPos)
{
	bool rollback = false;

    unsigned int i=0;
	for (i=0;i<r->selectedRules.size();i++)
	{
		rule *rr = r->selectedRules[i];

		if (selectorPos >= rr->GetNbAtoms())
			continue;

		atom *a = rr->GetAtom(selectorPos);
		unsigned int  id;
		
		//	We must handle the case where the rule is
		//	an alternate and all atoms are simple atoms :
		//	each atomID must be added to the selectors
		//	( this case can occur only when selectorPos == 0 )
		rule *rule_atomList = NULL;

		//	Search the deepest atom_id to select the rule:
		//	case of a macroatom ( rule )
		if ( a->GetRule() != NULL)
		{
			atom *aa = a;
			//	if first element is an alternate, we must
			//	find the first atoms of each possible rule
			if (aa->GetRule()->IsSequence() == false)
			{
				rule_atomList = DeepAtomSearch(aa);
				aa = rule_atomList->GetAtom(0);
			}
			//	otherwise, let's just retrieve the first atom
			else while (aa->GetRule() != NULL)
			{
				//	Are we are in the case where the rule is
				//	an alternate and all atoms are simple atoms ?
				if (aa->GetRule()->IsSequence() == false)
					rule_atomList = aa->GetRule();
				else
					rule_atomList = NULL;
				aa = aa->GetRule()->GetAtom(0);
			}

			id = aa->ID();
		}
		//	case of a simple atom
		else
		{
			//	We are in the case where the rule is
			//	an alternate and all atoms are simple atoms
			if (rr->IsSequence() == false)
			{
				if ((a->GetRule() == NULL) && (selectorPos > 0))
				{
					rollback = true;
					continue;
				}

				rule_atomList = rr;
				a = rr->GetAtom(0);
			}

			id = a->ID();
		}
		
		atom *aa;
		unsigned int atompos = 1;

		do
		{
			bool found = false;
			unsigned int pos = 0;
			aa = NULL;
			//	Conflicts should be detected here !!!
			//	i.e. : atom ID == first atomID of rule
			//	or rule IDs are equal but selector is different
			while ((!found)&&(pos<r->ruleSelectors.size()))
				found = (id == r->ruleSelectors[pos++]->selectorID);

			if (found)
				r->ruleSelectors[pos-1]->selectedRules.push_back(rr);
			else
			{
				rule_selector *rs = new rule_selector;

				rs->selectorID = id;
				rs->selectedRules.push_back(rr);
				rs->rollback = rollback;
				rs->selector = a->GetRule();

				r->ruleSelectors.push_back(rs);
			}
		
			if (rule_atomList != NULL)
			{
				if (atompos < rule_atomList->GetNbAtoms())
				{
					aa = rule_atomList->GetAtom(atompos++);
					id = aa->ID();
				}
			}
		}
		while (aa != NULL);
	}

	for (i=0;i<r->ruleSelectors.size();i++)
	{
		rule_selector *rs = r->ruleSelectors[i];
		if (rs->selectedRules.size()>1)
			BuildRuleSelector(rs,selectorPos+1);
	}
}

rule_selector *BuildRuleSelectors(rule *r)
{
	rule_selector *root = new rule_selector;
	root->selectorID = r->ID();
	root->selector = r;
	
	for (unsigned int i=0;i<r->GetNbAtoms();i++)
		root->selectedRules.push_back(r->GetAtom(i)->GetRule());
	
	BuildRuleSelector(root,0);

	return root;
}

void DeleteRuleSelectors(rule_selector *r)
{
	for (unsigned int i=0;i<r->ruleSelectors.size();i++)
		DeleteRuleSelectors(r->ruleSelectors[i]);

	delete r;
}

