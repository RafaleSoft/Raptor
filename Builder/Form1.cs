using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace Builder
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            InitTree();
        }

        private void InitTree()
        {
            System.Windows.Forms.TreeView tree = getTreeExtensions();
	        tree.BeginUpdate();

	        TreeNode GL_root = tree.Nodes.Add(GL_Label);
	        TreeNode GL_profile_root = tree.Nodes.Add(GLProfile_Label);
	        TreeNode VK_root = tree.Nodes.Add(VK_Label);
	        TreeNode ARB_root = tree.Nodes.Add(ARB_Label);
	        TreeNode VK_KHR_root = tree.Nodes.Add(VK_KHR_Label);
	        TreeNode ATI_root = tree.Nodes.Add(ATI_Label);
	        TreeNode EXT_root = tree.Nodes.Add(EXT_Label);
	        TreeNode NV_root = tree.Nodes.Add(NV_Label);
	        TreeNode KHR_root = tree.Nodes.Add(KHR_Label);
	        TreeNode _3DFX_root = tree.Nodes.Add(_3DFX_Label);
	        TreeNode OES_root = tree.Nodes.Add(OES_Label);
	        TreeNode WGL_root = tree.Nodes.Add(WGL_Label);

            Graphics g = CreateGraphics();
            System.IntPtr hDC = g.GetHdc();

            bool res = NativeWrapper.glvkInitBuilder(hDC);

            g.ReleaseHdc(hDC);
            g.Dispose();

            if (res)
            {
                EXTENSION[] exts = NativeWrapper.queryExtensions();
                for (uint i=0;i<exts.Count();i++)
	            {
		            EXTENSION extension = exts[i];
                    TreeNode root = null;
                    
		            switch(extension.kind)
		            {
                        case EXTENSION_KIND.COREGL:
                            root = GL_root;
                            break;
                        case EXTENSION_KIND.GLPROFILE:
                            root = GL_profile_root;
                            break;
                        case EXTENSION_KIND.COREVK:
                            root = VK_root;
                            break;
                        case EXTENSION_KIND.ARB:
                            root = ARB_root;
                            break;
                        case EXTENSION_KIND.VK:
                            root = VK_KHR_root;
                            break;
                        case EXTENSION_KIND.ATI:
                            root = ATI_root;
                            break;
                        case EXTENSION_KIND.EXT:
                            root = EXT_root;
                            break;
                        case EXTENSION_KIND.NV:
                            root = NV_root;
                            break;
                        case EXTENSION_KIND.KHR:
                            root = KHR_root;
                            break;
                        case EXTENSION_KIND.WGL:
                            root = WGL_root;
                            break;
                        case EXTENSION_KIND._3DFX:
                            root = _3DFX_root;
                            break;
                        case EXTENSION_KIND.OES:
                            root = OES_root;
                            break;
		            }

                    if (null != root)
                    {
                        TreeNode item = root.Nodes.Add(extension.extensionName);
                        item.Checked = extension.active;
                        item.Tag = extension.kind;
                    }
 	            }
            }
               
            tree.Sort();
            tree.ExpandAll();
            tree.EndUpdate();
        }

        private void OK_Click(object sender, EventArgs e)
        {

        }

        private void Cancel_Click(object sender, EventArgs e)
        {

        }

        private void SelectAll_Click(object sender, EventArgs e)
        {

        }

        private void SelectNone_Click(object sender, EventArgs e)
        {

        }

        private static System.String GL_Label = "CORE OpenGL versions";
		private static System.String GLProfile_Label = "OpenGL profiles";
		private static System.String VK_Label = "CORE Vulkan versions";
		private static System.String ARB_Label = "ARB extensions";
		private static System.String VK_KHR_Label = "Vulkan extensions";
		private static System.String ATI_Label = "ATI extensions";
		private static System.String EXT_Label = "EXT extensions";
		private static System.String NV_Label = "nVidia extensions";
		private static System.String KHR_Label = "Khronos extensions";
		private static System.String _3DFX_Label = "3DFX extensions";
		private static System.String WGL_Label = "WGL extensions";
		private static System.String OES_Label = "OpenGL ES extensions";
    }
}
