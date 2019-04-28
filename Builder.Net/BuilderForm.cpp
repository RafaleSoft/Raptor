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
#include <GL\gl.h>
#include <vcclr.h>
#include "vulkan.h"

#if !defined(AFX_GLBUILDER_H__0F2DA94E_7293_494C_B936_8CB72098E626__INCLUDED_)
	#include "GLBuilder.h"
#endif
#if !defined(__SIMDMACROS_H__)
	#include "SimdLib\simdMacros.h"
#endif

using namespace BuilderNet;

static CGLBuilder	builder;
typedef const char * (APIENTRY * pfnc)(HDC hdc);

// tip to solve warning LNK4248: unresolved typeref token:
//	the types below are only predeclarations in vulkan.h
//	so here, define only the name ...
struct VkPhysicalDevice_T {};
struct VkInstance_T {};

int glCreateContext(HDC& hdc,HGLRC& glhrc)
{
	DWORD	flags;
	BYTE	alphabits = 8;
	BYTE	depthbits = 24;
	BYTE	stencilbits = 8;
		
	flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE | PFD_DOUBLEBUFFER;

	PIXELFORMATDESCRIPTOR
	pfd = { 
			sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
			1,                     // version number 
			flags,
			PFD_TYPE_RGBA,         // RGBA type 
			24,                    // 24-bit color depth 
			0, 0, 0, 0, 0, 0,      // color bits ignored 
			alphabits,             // 8 bits alpha buffer 
			0,                     // alpha shift bit
			0,                     // no accumulation buffer 
			0, 0, 0, 0,            // accum bits ignored 
			depthbits,             // 16/24/32-bit z-buffer	 
			stencilbits,           // 8 bits stencil buffer 
			0,                     // no auxiliary buffer 
			PFD_MAIN_PLANE,        // main layer 
			0,                     // reserved 
			0, 0, 0                // layer masks ignored 
		};

	int pixelformat = 0;
	if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0 ) 
	{
		return -1; 
	}

	if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
	{ 
		return -1; 
	}

	glhrc = wglCreateContext(hdc);
	if (!glhrc)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

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


	const vector<CGLBuilder::EXTENSION>& extensions = builder.getExtensions();
	for (unsigned int i=0;i<extensions.size();i++)
	{
		const CGLBuilder::EXTENSION& extension = extensions[i];
		TreeNode^ item = nullptr;
		System::String ^str = gcnew System::String(extension.extensionName.data());
		switch(extension.kind)
		{
			case CGLBuilder::COREGL:
				item = GL_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::GLPROFILE:
				item = GL_profile_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::COREVK:
				item = VK_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::ARB:
				item = ARB_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::VK:
				item = VK_KHR_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::ATI:
				item = ATI_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::EXT:
				item = EXT_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::NV:
				item = NV_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::KHR:
				item = KHR_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::WGL:
				item = WGL_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::_3DFX:
				item = _3DFX_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
			case CGLBuilder::OES:
				item = OES_root->Nodes->Add(str);
				item->Tag = gcnew System::Int32(extension.kind);
				break;
		}
	}


	tree->Sort();
	tree->ExpandAll();
	tree->EndUpdate();

	
	Graphics^ g = CreateGraphics();
	HGLRC glhrc;
	System::IntPtr hDC = g->GetHdc();
	HDC dc = (HDC)(hDC.ToPointer());
	int res = glCreateContext(dc,glhrc);

	if (res == 0)
	{
		BOOL bres = wglMakeCurrent(dc,glhrc);

		string glextensions = (const char*)glGetString(GL_EXTENSIONS);
		string version = (const char*)glGetString(GL_VERSION);

		pfnc _glGetExtensionsStringARB;
		if (NULL != (_glGetExtensionsStringARB = (pfnc)wglGetProcAddress("wglGetExtensionsStringARB")))
		{
			string wgl_extensions = (const char*)_glGetExtensionsStringARB(dc);
			glextensions += wgl_extensions;
		}
		glextensions += " ";
		glextensions += CGLBuilder::full_profile;

		wchar_t buffer[MAX_PATH];
		GetEnvironmentVariable(L"VULKAN_BIN_PATH",(LPTSTR)buffer,MAX_PATH);
		std::wstring vkpath = buffer;
		vkpath += L"\\VULKAN-1.DLL";

		HMODULE module = LoadLibrary(vkpath.c_str());
		if (module != NULL)
		{
			PFN_vkCreateInstance vkCreateInstance = (PFN_vkCreateInstance)(GetProcAddress(module,"vkCreateInstance"));
			VkInstanceCreateInfo instanceCreateInfo = {	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
														NULL,                                   // const void* pNext;
														0,                                      // VkInstanceCreateFlags flags;
														NULL,                                   // const VkApplicationInfo* pApplicationInfo;
														0,                                      // uint32_t enabledLayerNameCount;
														NULL,                                   // const char* const* ppEnabledLayerNames;
														0,                                      // uint32_t enabledExtensionNameCount;
														NULL};									// const char* const* ppEnabledExtensionNames;
			VkInstance inst = NULL;
			if (VK_SUCCESS == vkCreateInstance(&instanceCreateInfo, NULL, &inst))
			{
				glextensions += " VK_VERSION_1_0";
				
				PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = 
					(PFN_vkEnumerateInstanceExtensionProperties)(GetProcAddress(module,"vkEnumerateInstanceExtensionProperties"));
				uint32_t pPropertyCount = 0;
				if (VK_SUCCESS == vkEnumerateInstanceExtensionProperties(NULL,&pPropertyCount,NULL))
				{
					VkExtensionProperties *pProperties = new VkExtensionProperties[pPropertyCount];
					vkEnumerateInstanceExtensionProperties(NULL,&pPropertyCount,pProperties);
					for (uint32_t i=0;i<pPropertyCount;i++)
					{
						glextensions += " ";
						glextensions += pProperties[i].extensionName;
					}
					delete [] pProperties;
				}

				if (VK_SUCCESS == res)
				{
					PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = 
						(PFN_vkEnumeratePhysicalDevices)(GetProcAddress(module,"vkEnumeratePhysicalDevices"));

					uint32_t pPhysicalDeviceCount = 0;
					res = vkEnumeratePhysicalDevices(inst,&pPhysicalDeviceCount,NULL);
					if ((VK_SUCCESS == res) && (pPhysicalDeviceCount > 0))
					{
						VkPhysicalDevice *m_pPhysicalDevices = new VkPhysicalDevice[pPhysicalDeviceCount];
						res = vkEnumeratePhysicalDevices(inst,&pPhysicalDeviceCount,m_pPhysicalDevices);
						if (VK_SUCCESS == res)
						{
							PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = 
								(PFN_vkEnumerateDeviceExtensionProperties)(GetProcAddress(module,"vkEnumerateDeviceExtensionProperties"));
							for (uint32_t i=0;i<pPhysicalDeviceCount;i++)
							{
								pPropertyCount = 0;
								res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i],NULL,&pPropertyCount,NULL);
								if ((VK_SUCCESS == res) && (pPropertyCount > 0))
								{
									VkExtensionProperties* pProperties = new VkExtensionProperties[pPropertyCount];
									res = vkEnumerateDeviceExtensionProperties(m_pPhysicalDevices[i],NULL,&pPropertyCount,pProperties);
									for (uint32_t j=0;j<pPropertyCount;j++)
									{
										glextensions += " ";
										glextensions += pProperties[j].extensionName;
									}
									delete [] pProperties;
								}
							}
						}

						delete [] m_pPhysicalDevices;
					}
				}		
			
				PFN_vkDestroyInstance vkDestroyInstance = (PFN_vkDestroyInstance)(GetProcAddress(module,"vkDestroyInstance"));
				vkDestroyInstance(inst,NULL);
			}
			FreeLibrary(module);
		}
		
		bres = wglMakeCurrent(NULL, NULL);
		bres = wglDeleteContext(glhrc);

		glextensions += builder.setVersion(version);
		builder.glQueryExtensions(glextensions);
	}

	const CPU_INFO& cpu = getCPUINFO();

	builder.activateExtension("RAPTOR_SMP_CODE_GENERATION", cpu.CPUID);
	builder.activateExtension("RAPTOR_SSE_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE));
	builder.activateExtension("RAPTOR_SSE2_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE2));
	builder.activateExtension("RAPTOR_SSE3_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE3));
	builder.activateExtension("RAPTOR_SSSE3_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSSE3));
	builder.activateExtension("RAPTOR_SSE41_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE41));
	builder.activateExtension("RAPTOR_SSE42_CODE_GENERATION", cpu.hasFeature(CPU_INFO::SSE42));
	builder.activateExtension("RAPTOR_AES_CODE_GENERATION", cpu.hasFeature(CPU_INFO::AES));
	builder.activateExtension("RAPTOR_AVX_CODE_GENERATION", cpu.hasFeature(CPU_INFO::AVX));
	builder.activateExtension("RAPTOR_FMA_CODE_GENERATION", cpu.hasFeature(CPU_INFO::FMA));

	g->ReleaseHdc(hDC);
	delete g;

	setData();
}

BuilderForm::~BuilderForm(void)
{
}

void BuilderForm::onOK(void)
{
	getData();

	if (!builder.checkConsistency(false))
		MessageBox::Show("Inconsistency found in extension profile. Header file not generated.");
    else
	    builder.writeHeader("CodeGeneration.h");

	this->Close();
}

void BuilderForm::onCancel(void)
{
	this->Close();
}

void BuilderForm::onSelectAll() 
{
	builder.activateAll();

    setData();
}

void BuilderForm::onSelectNone() 
{
	builder.activateNone();

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

			node->Checked = builder.isExtensionActive(ch);

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


	SMP->Checked = (builder.isExtensionActive("RAPTOR_SMP_CODE_GENERATION") ? true : false);
	SSE->Checked = (builder.isExtensionActive("RAPTOR_SSE_CODE_GENERATION") ? true : false);
	SSE2->Checked = (builder.isExtensionActive("RAPTOR_SSE2_CODE_GENERATION") ? true : false);
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

			builder.activateExtension(ch,node->Checked);

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

    builder.activateExtension("RAPTOR_SMP_CODE_GENERATION", SMP->Checked);
    builder.activateExtension("RAPTOR_SSE_CODE_GENERATION", SSE->Checked);
    builder.activateExtension("RAPTOR_SSE2_CODE_GENERATION", SSE2->Checked);
	builder.activateDebug(BUILD_DEBUG->Checked);
	builder.activateRedist(BUILD_REDIST->Checked);
	builder.activateCompute(BUILD_COMPUTE->Checked);
}
