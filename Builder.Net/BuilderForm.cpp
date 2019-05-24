/***************************************************************************/
/*                                                                         */
/*  BuilderForm.cpp                                                        */
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
#include "BuilderForm.h"
#include <windows.h>
#include <vcclr.h>

#include <BuilderNative.h>
using namespace BuilderNet;


BuilderForm::BuilderForm(void)
{
	System::Windows::Forms::TreeView^ tree = getTreeExtensions();
	tree->BeginUpdate();

	TreeNode^ GL_root = tree->Nodes->Add(GL_Label);
	TreeNode^ GL_profile_root = tree->Nodes->Add(GLProfile_Label);
	TreeNode^ VK_root = tree->Nodes->Add(VK_Label);
	TreeNode^ ARB_root = tree->Nodes->Add(ARB_Label);
	TreeNode^ VK_KHR_root = tree->Nodes->Add(VK_KHR_Label);
	TreeNode^ ATI_root = tree->Nodes->Add(ATI_Label);
	TreeNode^ EXT_root = tree->Nodes->Add(EXT_Label);
	TreeNode^ NV_root = tree->Nodes->Add(NV_Label);
	TreeNode^ KHR_root = tree->Nodes->Add(KHR_Label);
	TreeNode^ _3DFX_root = tree->Nodes->Add(_3DFX_Label);
	TreeNode^ OES_root = tree->Nodes->Add(OES_Label);
	TreeNode^ WGL_root = tree->Nodes->Add(WGL_Label);

	Graphics^ g = CreateGraphics();
	System::IntPtr hDC = g->GetHdc();
	HDC dc = (HDC)(hDC.ToPointer());
	bool res = glvkInitBuilder(dc);


	NATIVE_EXTENSION* extensions = NULL;
	uint32_t s = 0;
	if (getExtensions(NULL, &s))
	{
		extensions = new NATIVE_EXTENSION[s];
		res = getExtensions(extensions, &s);
	}

	for (uint32_t i=0;i<s;i++)
	{
		const NATIVE_EXTENSION& extension = extensions[i];
		TreeNode^ root = nullptr;

		switch(extension.kind)
		{
			case EXTENSION_KIND::COREGL:
				root = GL_root;
				break;
			case EXTENSION_KIND::GLPROFILE:
				root = GL_profile_root;
				break;
			case EXTENSION_KIND::COREVK:
				root = VK_root;
				break;
			case EXTENSION_KIND::ARB:
				root = ARB_root;
				break;
			case EXTENSION_KIND::VK:
				root = VK_KHR_root;
				break;
			case EXTENSION_KIND::ATI:
				root = ATI_root;
				break;
			case EXTENSION_KIND::EXT:
				root = EXT_root;
				break;
			case EXTENSION_KIND::NV:
				root = NV_root;
				break;
			case EXTENSION_KIND::KHR:
				root = KHR_root;
				break;
			case EXTENSION_KIND::WGL:
				root = WGL_root;
				break;
			case EXTENSION_KIND::_3DFX:
				root = _3DFX_root;
				break;
			case EXTENSION_KIND::OES:
				root = OES_root;
				break;
			case EXTENSION_KIND::CPU:
				if (!strcmp(extension.extensionName,"RAPTOR_SMP_CODE_GENERATION"))
					SMP->Checked = true;
				else if (!strcmp(extension.extensionName,"RAPTOR_SSE_CODE_GENERATION"))
					SSE->Checked = true;
				else if (!strcmp(extension.extensionName,"RAPTOR_SSE2_CODE_GENERATION"))
					SSE2->Checked = true;
				break;
		}

		if (nullptr != root)
		{
			System::String ^str = gcnew System::String(extension.extensionName);
			TreeNode^ item = root->Nodes->Add(str);
			item->Tag = gcnew System::Int32(extension.kind);
			item->Checked = extension.active;
		}
	}

	tree->Sort();
	tree->ExpandAll();
	tree->EndUpdate();
	
	freeExtensions(extensions, s);
	delete[] extensions;

	g->ReleaseHdc(hDC);
	delete g;
}

BuilderForm::~BuilderForm(void)
{
}

void BuilderForm::onOK(void)
{
	getData();

	if (!checkConsistency(false))
		MessageBox::Show("Inconsistency found in extension profile. Header file not generated.");
    else
	    writeHeader("CodeGeneration.h");

	this->Close();
}

void BuilderForm::onCancel(void)
{
	this->Close();
}

void BuilderForm::onSelectAll() 
{
	activateAllOrNone(true);

    setData();
}

void BuilderForm::onSelectNone() 
{
	activateAllOrNone(false);

    setData();
}

void BuilderForm::setData(void)
{
	IEnumerator^ it = getTreeExtensions()->Nodes->GetEnumerator();
	it->MoveNext();
	TreeNode^ node = (TreeNode^)it->Current;

	while (node != nullptr)
    {
		if (node->Tag != nullptr)
		{
			System::String^ name = node->Text;
			pin_ptr<const wchar_t> wch = PtrToStringChars(name);

			size_t convertedChars = 0;
			size_t  sizeInBytes = ((name->Length + 1) * 2);
			char    *ch = (char *)malloc(sizeInBytes);

			wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

			node->Checked = isExtensionActive(ch);

			free(ch);
		}
        
		TreeNode^ child = node->FirstNode;
        if (child != nullptr)
            node = child;
        else
        {
			TreeNode^ next = node->NextNode;
            if (next != nullptr)
                node = next;
            else
            {
				TreeNode^ parent = node->Parent;
				node = parent->NextNode;
            }
        }
    }

	SMP->Checked = (isExtensionActive("RAPTOR_SMP_CODE_GENERATION") ? true : false);
	SSE->Checked = (isExtensionActive("RAPTOR_SSE_CODE_GENERATION") ? true : false);
	SSE2->Checked = (isExtensionActive("RAPTOR_SSE2_CODE_GENERATION") ? true : false);
}


void BuilderForm::getData(void)
{
	IEnumerator^ it = getTreeExtensions()->Nodes->GetEnumerator();
	it->MoveNext();
	TreeNode^ node = (TreeNode^)it->Current;

	while (node != nullptr)
    {
		if (node->Tag != nullptr)
		{
			System::String^ name = node->Text;
			pin_ptr<const wchar_t> wch = PtrToStringChars(name);

			size_t convertedChars = 0;
			size_t  sizeInBytes = ((name->Length + 1) * 2);
			char    *ch = (char *)malloc(sizeInBytes);

			wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

			activateExtension(ch,node->Checked);

			free(ch);
		}
        
        TreeNode^ child = node->FirstNode;
        if (child != nullptr)
            node = child;
        else
        {
			TreeNode^ next = node->NextNode;
            if (next != nullptr)
                node = next;
            else
            {
				TreeNode^ parent = node->Parent;
				node = parent->NextNode;
            }
        }   
    }

    activateExtension("RAPTOR_SMP_CODE_GENERATION", SMP->Checked);
    activateExtension("RAPTOR_SSE_CODE_GENERATION", SSE->Checked);
    activateExtension("RAPTOR_SSE2_CODE_GENERATION", SSE2->Checked);

	BUILD_SUPPLEMENT supplement = { BUILD_DEBUG->Checked, BUILD_REDIST->Checked, BUILD_COMPUTE->Checked };
	setBuildSupplement(&supplement);
}
