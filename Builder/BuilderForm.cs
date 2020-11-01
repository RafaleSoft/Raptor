/***************************************************************************/
/*                                                                         */
/*  Form1.cs                                                               */
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


using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace Builder
{
    public partial class BuilderForm : Form
    {
        public BuilderForm()
        {
            InitializeComponent();
            InitTree();
        }

        private void InitTree()
        {
            Graphics g = CreateGraphics();
            System.IntPtr hDC = g.GetHdc();

            bool res = NativeWrapper.glvkInitBuilder(hDC);

            g.ReleaseHdc(hDC);
            g.Dispose();


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
                        case EXTENSION_KIND.CPU:
                            if (extension.extensionName == "RAPTOR_SMP_CODE_GENERATION")
                                SMP.Checked = true;
                            else if (extension.extensionName == "RAPTOR_SSE_CODE_GENERATION")
                                SSE.Checked = true;
                            else if (extension.extensionName == "RAPTOR_SSE2_CODE_GENERATION")
                                SSE2.Checked = true;
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
           	getData();

	        if (!NativeWrapper.checkConsistency(false))
		        MessageBox.Show("Inconsistency found in extension profile. Header file not generated.",
                                "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            else
	            NativeWrapper.writeHeader("CodeGeneration.h");

	        Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void SelectAll_Click(object sender, EventArgs e)
        {
            NativeWrapper.activateAllOrNone(true);
            setData();
        }

        private void SelectNone_Click(object sender, EventArgs e)
        {
            NativeWrapper.activateAllOrNone(false);
            setData();
        }

        private void getData()
        {
            System.Windows.Forms.TreeView tree = getTreeExtensions();
	        IEnumerator it = tree.Nodes.GetEnumerator();

            it.MoveNext();
            TreeNode node = (TreeNode)it.Current;

            while (null != node)
            {
		        if (null != node.Tag)
                    NativeWrapper.activateExtension(node.Text, node.Checked);
        
                TreeNode child = node.FirstNode;
                if (null != child)
                    node = child;
                else
                {
			        TreeNode next = node.NextNode;
                    if (null != next)
                        node = next;
                    else
                    {
				        TreeNode parent = node.Parent;
				        node = parent.NextNode;
                    }
                }
            }

            NativeWrapper.activateExtension("RAPTOR_SMP_CODE_GENERATION", SMP.Checked);
            NativeWrapper.activateExtension("RAPTOR_SSE_CODE_GENERATION", SSE.Checked);
            NativeWrapper.activateExtension("RAPTOR_SSE2_CODE_GENERATION", SSE2.Checked);

            BUILD_SUPPLEMENT bld = new BUILD_SUPPLEMENT();
            bld.DEBUG = BUILD_DEBUG.Checked;
            bld.REDIST = BUILD_REDIST.Checked;
            bld.COMPUTE = BUILD_COMPUTE.Checked;
            bld.PHYSX = BUILD_PHYSX.Checked;
            NativeWrapper.setBuildSupplement(ref bld);
        }

        private void setData()
        {
            EXTENSION[] exts = NativeWrapper.queryExtensions();

            System.Windows.Forms.TreeView tree = getTreeExtensions();
            IEnumerator it = tree.Nodes.GetEnumerator();

            it.MoveNext();
            TreeNode node = (TreeNode)it.Current;

            while (null != node)
            {
                if ((null != node.Tag) && (node.Text.Length > 1))
                {
                    EXTENSION ext = exts.First(e => e.extensionName == node.Text);
                    node.Checked = ext.active;
                }

                TreeNode child = node.FirstNode;
                if (null != child)
                    node = child;
                else
                {
                    TreeNode next = node.NextNode;
                    if (null != next)
                        node = next;
                    else
                    {
                        TreeNode parent = node.Parent;
                        node = parent.NextNode;
                    }
                }
            }

            {
                EXTENSION ext = exts.First(e => e.extensionName == "RAPTOR_SMP_CODE_GENERATION");
                SMP.Checked = ext.active;
                ext = exts.First(e => e.extensionName == "RAPTOR_SSE_CODE_GENERATION");
                SSE.Checked = ext.active;
                ext = exts.First(e => e.extensionName == "RAPTOR_SSE2_CODE_GENERATION");
                SSE2.Checked = ext.active;
            }
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
