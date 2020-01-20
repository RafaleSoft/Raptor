/***************************************************************************/
/*                                                                         */
/*  Automat.cpp                                                            */
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
#include "Automat.h"

//	match rules
bool MatchRule(string r1,string r2)
{
	if (r1[0] == '\\')
		if ((r1[1] == '+')||(r1[1]=='*')||(r1[1]=='?'))
			r1 = r1.substr(2);

	if (r2[0] == '\\')
		if ((r2[1] == '+')||(r2[1]=='*')||(r1[1]=='?'))
			r2 = r2.substr(2);

	return (r1==r2);
}

//	Return true if r1 is fully included in r2
bool IsFullyIncluded(string r1,string r2)
{
	if (r1[0] == '\\')
		if ((r1[1] == '+')||(r1[1]=='*')||(r1[1]=='?'))
			r1 = r1.substr(2);

	if (r2[0] == '\\')
		if ((r2[1] == '+')||(r2[1]=='*')||(r1[1]=='?'))
			r2 = r2.substr(2);

	bool included = true;
	for (unsigned int i=0;i<r1.size();i++)
	{
		included &= (string::npos != r2.find(r1[i]));
	}

	return included;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutomat::CAutomat():
	branchLength(0),branchLoop(NULL),terminator(NULL)
{

}

CAutomat::~CAutomat()
{

}

//	
//	Writes the terminator branch of
//	a node. If parsing go trough that
//	branch, an atom has been parsed
//	successfully
void CAutomat::TerminateBranch(branch *&bb,atom *a)
{
	//	terminate branch with termination node
	bool noError = terminator == NULL;

	if (noError)
	{
		node *nn = new node;
		bb->nextNode = nn;
		nn->terminate = a->ID();
		nn->length = branchLength;
		terminator = a;
	}
	else
	{
		cout << "Warning: atoms '" << a->Name() << "' and '";
		cout << terminator->Name() << "' generate a conflict. Atom parsed will be ";
		cout << terminator->Name() << endl;

		delete bb;
		bb = NULL;
	}
}



//
//	Add a branch to an array of branches
//	and returns true is branch is added
bool CAutomat::AddBranch(vector<branch*> &branchesArray,branch *b)
{
	bool match = false;
	unsigned int k=0;

	branch *bb2 = NULL;

	while ((!match) && (k<branchesArray.size()))
	{
		bb2 = branchesArray[k++];
		match = (b->branchChars==bb2->branchChars);
	}

	if (!match)
	{
		branchesArray.push_back(b);
		return true;
	}
	else
	{
		for (unsigned int i=0;i<b->rulesSrc.size();i++)
			bb2->rulesSrc.push_back(b->rulesSrc[i]);
		return false;
	}
}

//	
//	Computes the length of an atom
int Length(atom *a)
{
	size_t maxlength = a->NbValidChars();
	unsigned int length = 0;

	for (size_t i=0; i < maxlength; i++)
	{
		string chrs = a->GetValidChars(i);

		if ((chrs[0] == '\\') && (chrs[1] == '*'))
		{
			length = 65535;
			break;
		}
		else
			length++;
	}

	//	if length > maxlength then this atom
	//	can therically have an infinite length
	//	due to variable length charsets
	return length;
}

//
//	Compute the possible chars for an atom
//	at a given length
//	The possible chars is computed using the following:
//	- it is the possible character using the minimum length
//	possible in variable length charsets
//	- if a variable charset is found before the desired position
//	it must be added
//	- if the desired position is beyond the length of the atom and
//	if this atom contains variable charsets, a terminator must
//	be added
bool PossibleChars(atom *a,unsigned int length,vector<string> &possibles)
{
	bool isInfinite = false;
	unsigned int pos = 0;
	unsigned int maxLength = a->NbValidChars() - 1;
	if (length < maxLength)
		maxLength = length;

	while (pos <= maxLength)
	{
		string chrs = a->GetValidChars(pos);

		//	if variable length charset, any length
		//	is correct, so add the possibility
		if ((chrs[0]=='\\')&&(chrs[1]=='*'))
		{
			isInfinite = true;
	
			if (pos >= maxLength) 
			{
				possibles.push_back(chrs);

				if (pos < (a->NbValidChars() - 1))
					possibles.push_back(a->GetValidChars(pos+1));
				else
					possibles.push_back("");	// null charset =>will make terminate branch
			}
			//	Desired position is beyond possible size
			if (length > maxLength)
				possibles.push_back("");

			//	We jump over the next position : it has been added
			//	if we are at the desired position.
			//	It can't be added if we are not yet at the desired
			//	position because we must use the minimum length
			//	for variable length charsets, i.e. 0
			pos++;
			if (maxLength < (a->NbValidChars() - 1))
				maxLength++;
		}
		else
		{
			if (isInfinite || (pos == length))
				possibles.push_back(chrs);
		}

		pos++;
	}

	//	At least a terminator must be returned
	//	and each charset must be unique to be
	//	further factorized
	if (possibles.size() == 0)
		possibles.push_back("");
	else
	{
		for (unsigned int i=0;i<possibles.size()-1;i++)
		{
			string &chrs = possibles[i];
			for (unsigned int k=1;k<possibles.size();k++)
			{
				if (possibles[k] == chrs)
					possibles.erase(possibles.begin()+k);
			}
		}
	}

	return true;
}

//
//	This method factorize branches to ensure that
//	the automat is determinist. When branches are
//	factorized, the rules are combined. So if we
//	reach the case where a node will have two 
//	terminate branches, we are in a case of ambiguity,
//	It is impossible to make the automat determinist.
void Factorize(vector<branch*> &factorBranches)
{
	if (factorBranches.size() < 1)
		return;

	for (unsigned int k=0;k<factorBranches.size()-1;k++)
	{
		branch *bb = factorBranches[k];

		for (unsigned int l=k+1;l<factorBranches.size();l++)
		{
			branch *bbb = factorBranches[l];

			string::size_type pos = 0;
			if (string::npos != (pos = bb->branchChars.find(bbb->branchChars)))
			{
				bb->branchChars.erase(pos,bbb->branchChars.size());
				for (unsigned int i=0;i<bb->rulesSrc.size();i++)
					bbb->rulesSrc.push_back(bb->rulesSrc[i]);
			}
			else if (string::npos != (pos = bbb->branchChars.find(bb->branchChars)))
			{
				bbb->branchChars.erase(pos,bb->branchChars.size());
				for (unsigned int i=0;i<bbb->rulesSrc.size();i++)
					bb->rulesSrc.push_back(bbb->rulesSrc[i]);
			}
			else if (IsFullyIncluded(bbb->branchChars,bb->branchChars))
			{
				unsigned int i=0;
				for (i=0;i<bbb->branchChars.size();i++)
				{
					pos = bb->branchChars.find(bbb->branchChars[i]);
					bb->branchChars.erase(pos,1);
				}
				for (i=0;i<bb->rulesSrc.size();i++)
					bbb->rulesSrc.push_back(bb->rulesSrc[i]);
			}
			else if (IsFullyIncluded(bb->branchChars,bbb->branchChars))
			{
				unsigned int i=0;
				for (i=0;i<bb->branchChars.size();i++)
				{
					pos = bbb->branchChars.find(bb->branchChars[i]);
					bbb->branchChars.erase(pos,1);
				}
				for (i=0;i<bbb->rulesSrc.size();i++)
					bb->rulesSrc.push_back(bbb->rulesSrc[i]);
			}
			// There is another case: the rules are partially included.
			// we then need to create a new branch.
		}
	}
}

//
//	This method computes all possibles branches for
//	a given node. The node is given during BuildNodes
//	and the new branches will then be processed continuing
//	BuildNodes inner loops
int CAutomat::BuildNextBranches(atom* a,int bLength,node* n,vector<branch*> &nextBranches,vector<branch*> &factorBranches,int nbr)
{
	vector<string> possibles;
	PossibleChars(a,bLength,possibles);

	for (unsigned int j=0;j<possibles.size();j++)
	{
		branch *bb = new branch;
		bb->branchChars = possibles[j];
		bb->rulesSrc.push_back(a);

		//
		//	1st case :	the branch points to the terminator
		//				of the atom, no further chars are wanted
		if (bb->branchChars.empty())
		{
			TerminateBranch(bb,a);
			if (bb != NULL)
				n->branches.push_back(bb);
		}
		//
		//	2nd case :	the branch holds a variable length
		//				charset. We must try to connect to an
		//				existing node to reduce the automat.
		//				If no existing node is found, then
		//				we build the loop by pointing to self
		else if ((bb->branchChars[0]=='\\')&&(bb->branchChars[1]=='*'))
		{
			bb->branchChars.erase(0,2);

			if (AddBranch(n->branches,bb))
			{
				factorBranches.push_back(bb);
			
				if (nbr == 1)
				{
					bb->nextNode = n;
					loopNodes.push_back(n);
				}
				else
				{
					unsigned int pos = 0;
					bool found = false;
					while ((!found) && (pos < loopNodes.size()))
					{
						node *nn = loopNodes[pos++];
						unsigned int pos2 = 0;
						while ((!found) && (pos2 < nn->branches.size()))
							found = bb->branchChars == nn->branches[pos2++]->branchChars;
						if (found)
							bb->nextNode = nn;
					}

					if (!found)
					{
						if (n->branches.size() == 1)
						{
							bb->nextNode = n;
							loopNodes.push_back(n);
						//cout << "Error: rule ambiguity for atom '" << LPCTSTR(a->name) << "'" << endl;
						//return -1;
						}
						else
						{
							bb->nextNode = new node;
							bb->nextNode->terminate = 0;
							bb->nextNode->length = n->length + 1;
							loopNodes.push_back(bb->nextNode);
							nextBranches.push_back(bb);
						}
					}
				}
			}
		}
		//
		//	3rd case :	The general case, we simply add the branch
		//				to the lists of the branches that will be
		//				processed by the caller of this method
		else
		{
			bb->nextNode = NULL;
			if (AddBranch(n->branches,bb))
			{
				factorBranches.push_back(bb);
				nextBranches.push_back(bb);
			}
		}
	}

	return 1;
}

//
//	This method build the automat tree that
//	that will parse all atoms of the language
int CAutomat::BuildNodes()
{
	vector<branch*>	nextBranches;
	nextBranches = branches;

	while (nextBranches.size()>0)
	{
		vector<branch*>	workBranches;
		workBranches = nextBranches;
		nextBranches.clear();

		branchLength++;

		for (unsigned int i=0;i<workBranches.size();i++)
		{
			branch *b = workBranches[i];

			// because of loops, the node can't be replaced
			if (b->nextNode == NULL)
				b->nextNode = new node;
			node *n = b->nextNode;

			unsigned int nbr = b->rulesSrc.size();
			
			n->length = branchLength;
			n->terminate = 0;

			branchLoop = NULL;
			terminator = NULL;

			vector<branch*>	factorBranches;

			for (unsigned int j=0;j<nbr;j++)
			{
				atom *a = (atom*)(b->rulesSrc[j]);
				if (0 > BuildNextBranches(a,branchLength,n,nextBranches,factorBranches,nbr))
					return -1;
			}

			// factorize branches
			Factorize(factorBranches);
		}		
	}

	return 0;
}

int CAutomat::BuildAutomat(void)
{
	if (MicroLexConfig::GetConfig()->Verbose())
		cout << "Building automat..." << endl;

	//	
	//	Process rules that start with an optional.
	//	This kind of syntax is anoying for the automat, so
	//	it will soon disapear. Use the <empty> tag in a rule
	//	to describe optional atoms.
	unsigned int i=0;
	for (i=0;i<atom::GetNbAtoms();i++)
	{
		atom *a = atom::GetAtom(i);

		if (a->GetValidChars(0)[0]=='\\')
		if (a->GetValidChars(0)[1]=='?')
		if (a->NbValidChars()<2)
		{
			cout << "Option tag '?' can't define rule " << a->Name() << endl;
			return -1;
		}
		else
		{
			atom *aa = new atom(a->Name(),
								a->ID(),
								a->GetRule(),
								a->MapType());
			for (unsigned int j=1;j<a->NbValidChars();j++)
				aa->AddValidChars(a->GetValidChars(j));
			atom::AddAtom(aa);
		}
	}

	vector<atom*> nextRules;
	for(i=0;i<atom::GetNbAtoms();i++)
		nextRules.push_back(atom::GetAtom(i));
	bool option = false;

	//
	//	Prepare the entry points of the automat
	//	( similar branches will be merged )
	while (nextRules.size()>0)
	{
		option = false;
		branch	*b = new branch;

		atom *a = nextRules.front();
		nextRules.erase(nextRules.begin());
		b->rulesSrc.push_back(a);
		b->nextNode = NULL;

		// extract head
		string rchars = a->GetValidChars(0);
		if (rchars[0] == '\\')
		{
			if (rchars[1]=='*')
				rchars = rchars.substr(2);
			else if (rchars[1]=='?')
				rchars = rchars.substr(2);
		}
		b->branchChars = rchars.data();

		//	find rules according to branch length
		//	and that are not processed yet
		size_t k=0;
		while(k<nextRules.size())
		{
			atom *a2 = nextRules[k];
			if (MatchRule(a2->GetValidChars(0),rchars))
			{
				nextRules.erase(nextRules.begin() + k);

				b->rulesSrc.push_back(a2);
			}
			else
				k++;
		}

		branches.push_back(b);
	}

	//	Try to factorize rules : all the branches
	//	that are in the pool must have a set of chars
	//	that is exclusive with all other branches.
	Factorize(branches);
	
	int res = BuildNodes();

	if (MicroLexConfig::GetConfig()->Verbose())
		cout << " done" << endl << endl;

	return res;
}


void delete_branch(branch *b,node *n);
void CAutomat::delete_node(node *n)
{
	if (n == NULL)
	{
		map<branch*,branch*>::const_iterator pos = mapBranches.begin( );
		branch *bb = NULL;
		while (pos != mapBranches.end())
		{
			bb = (*pos++).second;
			delete bb;
		}

		map<node*,node*>::const_iterator pos2 = mapNodes.begin( );
		node *nnn = NULL;
		while (pos2 != mapNodes.end())
		{	
			nnn = (*pos2++).second;
			delete nnn;
		}

		return;
	}

	for (unsigned int i=0;i<n->branches.size();i++)
		delete_branch(n->branches[i],n);

	map<node*,node*>::iterator res = mapNodes.find( n );
	if (res == mapNodes.end())
		mapNodes[n] = n;
}

void CAutomat::delete_branch(branch *b,node *n)
{
	if (b->nextNode != n)
		delete_node(b->nextNode);

	map<branch*,branch*>::iterator res = mapBranches.find( b );
	if (res == mapBranches.end())
		mapBranches[b] = b;
}

void CAutomat::DestroyBranches()
{
	if (branches.size()>0)
	{
		for (unsigned int i=0;i<branches.size();i++)
			delete_branch(branches[i],NULL);
		delete_node(NULL);
	}
}
