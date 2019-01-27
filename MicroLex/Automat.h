/***************************************************************************/
/*                                                                         */
/*  Automat.h                                                              */
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


#if !defined(AFX_AUTOMAT_H__FC1DC88F_DC35_4078_B1AF_2B9337F4DFAB__INCLUDED_)
#define AFX_AUTOMAT_H__FC1DC88F_DC35_4078_B1AF_2B9337F4DFAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MicroLex.h"
#include <map>
using namespace std;

class CAutomat  
{
public:
	CAutomat();
	virtual ~CAutomat();

	void DestroyBranches(void);
	int BuildAutomat(void);

	const vector<branch*> &getBranches(void) const  { return branches; };

private:
	//	current branch length processed for lexical analyser
	unsigned int				branchLength;
	//	a reverse loop to a node has been created
	branch				*branchLoop;
	//	get a reference to the last terminator created to detect rule conflicts 
	atom				*terminator;

	vector<branch*>	branches;
	vector<node*> loopNodes;

	//	Memory management for tree deletion
	map<branch*,branch*> mapBranches;
	map<node*,node*> mapNodes;

	void delete_node(node *n);
	void delete_branch(branch *b,node *n);

	bool AddBranch(vector<branch*> &branchesArray,branch *b);
	int BuildNextBranches(atom* a,int branchLength,node* n,vector<branch*> &nextBranches,vector<branch*> &factorBranches,int nbr);
	void TerminateBranch(branch *&bb,atom *a);
	int BuildNodes(void);
};

#endif // !defined(AFX_AUTOMAT_H__FC1DC88F_DC35_4078_B1AF_2B9337F4DFAB__INCLUDED_)
