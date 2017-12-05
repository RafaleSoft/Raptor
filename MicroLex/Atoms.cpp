#include "StdAfx.h"
#include "MicroLex.h"


vector<atom*>	atom::m_atoms;


atom::atom(	const string &name,
			unsigned int id,
			rule* macroAtom,
			const string &mapType)
{
	m_name = name;
	m_atomID = id;
	m_macroAtom = macroAtom;
	m_mapType = mapType;
}

atom::~atom()
{
}


bool atom::AddAtom(atom *a)
{
	if (a != NULL)
	{
//		if (atom::FindAtom(a->Name()) == NULL)
//		{
			m_atoms.push_back(a);
			return true;
//		}
//		else
//			return false;
	}
	else
		return false;
}

atom* atom::GetAtom(unsigned int pos)
{
	if (pos >= m_atoms.size())
		return NULL;
	else
		return m_atoms[pos];
}

atom* atom::FindAtom(const string& name)
{
	bool found=false;
	unsigned int i=0;

	while ((i<m_atoms.size()) && (!found))
		found = m_atoms[i++]->Name() == name;

	if (found)
		return m_atoms[i-1];
	else
		return NULL;
}

void atom::DestroyAtoms(void)
{
	if (m_atoms.size()>0)
	{
		for (unsigned int i=0;i<m_atoms.size();i++)
			delete m_atoms[i];
		m_atoms.clear();
	}
}

void atom::AddValidChars(const string& chars)
{
	m_validChars.push_back(chars);
}

void atom::SetValidChars(unsigned int pos,const string& chars)
{
	if (pos >= m_validChars.size())
		return;
	else
		m_validChars[pos]=chars;
}

string	atom::GetValidChars(unsigned int pos)
{
	if (pos >= m_validChars.size())
		return "";
	else
		return m_validChars[pos];
}

//	Display atom content
void atom::Display(void)
{
	cout << "ATOM parsed:" << endl;
	cout << "	atom: " << m_name << endl;
	cout << "	  ID: " << m_atomID << endl;
	cout << "	   c: ";

	unsigned int i;

	if (m_validChars.size() > 0)
	{
		for (i=0;i<m_validChars.size()-1;i++)
			cout << m_validChars[i] << ",";
		cout << m_validChars[i] << endl;
	}
	else
		cout << "EMPTY !" << endl;

	cout << endl;
}

