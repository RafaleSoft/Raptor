#pragma once

#include "Form1.h"

namespace BuilderNet {

	public ref class BuilderForm : public Form1
	{
	public:
		BuilderForm(void);
		virtual ~BuilderForm(void);

	protected:
		virtual void onOK(void) override;
		virtual void onCancel(void) override;
		virtual void onSelectAll(void) override;
		virtual void onSelectNone(void) override;

	private:
		void setData(void);
		void getData(void);

		static System::String ^GL_Label = "CORE OpenGL versions";
		static System::String ^VK_Label = "CORE Vulkan versions";
		static System::String ^ARB_Label = "ARB extensions";
		static System::String ^VK_KHR_Label = "Vulkan extensions";
		static System::String ^ATI_Label = "ATI extensions";
		static System::String ^EXT_Label = "EXT extensions";
		static System::String ^NV_Label = "nVidia extensions";
		static System::String ^KHR_Label = "Khronos extensions";
		static System::String ^_3DFX_Label = "3DFX extensions";
		static System::String ^WGL_Label = "WGL extensions";
		static System::String ^OES_Label = "OpenGL ES extensions";
	};
}