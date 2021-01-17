/***************************************************************************/
/*                                                                         */
/*  Form1.designer.cs                                                      */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


namespace Builder
{
    partial class BuilderForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        System.Windows.Forms.TreeView getTreeExtensions()
		{
			return TREE_EXTENSIONS;
		}

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BuilderForm));
            this.OK = new System.Windows.Forms.Button();
            this.Cancel = new System.Windows.Forms.Button();
            this.SelectAll = new System.Windows.Forms.Button();
            this.SelectNone = new System.Windows.Forms.Button();
            this.Build = new System.Windows.Forms.GroupBox();
            this.BUILD_DEBUG = new System.Windows.Forms.CheckBox();
            this.BUILD_COMPUTE = new System.Windows.Forms.CheckBox();
            this.BUILD_REDIST = new System.Windows.Forms.CheckBox();
            this.CPU = new System.Windows.Forms.GroupBox();
            this.SSE2 = new System.Windows.Forms.CheckBox();
            this.SSE = new System.Windows.Forms.CheckBox();
            this.SMP = new System.Windows.Forms.CheckBox();
            this.TREE_EXTENSIONS = new System.Windows.Forms.TreeView();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.BUILD_PHYSX = new System.Windows.Forms.CheckBox();
            this.Build.SuspendLayout();
            this.CPU.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // OK
            // 
            this.OK.Location = new System.Drawing.Point(430, 13);
            this.OK.Name = "OK";
            this.OK.Size = new System.Drawing.Size(75, 23);
            this.OK.TabIndex = 2;
            this.OK.Text = "OK";
            this.OK.UseVisualStyleBackColor = true;
            this.OK.Click += new System.EventHandler(this.OK_Click);
            // 
            // Cancel
            // 
            this.Cancel.Location = new System.Drawing.Point(430, 42);
            this.Cancel.Name = "Cancel";
            this.Cancel.Size = new System.Drawing.Size(75, 23);
            this.Cancel.TabIndex = 4;
            this.Cancel.Text = "Cancel";
            this.Cancel.UseVisualStyleBackColor = true;
            this.Cancel.Click += new System.EventHandler(this.Cancel_Click);
            // 
            // SelectAll
            // 
            this.SelectAll.Location = new System.Drawing.Point(349, 13);
            this.SelectAll.Name = "SelectAll";
            this.SelectAll.Size = new System.Drawing.Size(75, 23);
            this.SelectAll.TabIndex = 1;
            this.SelectAll.Text = "SelectAll";
            this.SelectAll.UseVisualStyleBackColor = true;
            this.SelectAll.Click += new System.EventHandler(this.SelectAll_Click);
            // 
            // SelectNone
            // 
            this.SelectNone.Location = new System.Drawing.Point(349, 42);
            this.SelectNone.Name = "SelectNone";
            this.SelectNone.Size = new System.Drawing.Size(75, 23);
            this.SelectNone.TabIndex = 3;
            this.SelectNone.Text = "SelectNone";
            this.SelectNone.UseVisualStyleBackColor = true;
            this.SelectNone.Click += new System.EventHandler(this.SelectNone_Click);
            // 
            // Build
            // 
            this.Build.Controls.Add(this.BUILD_PHYSX);
            this.Build.Controls.Add(this.BUILD_DEBUG);
            this.Build.Controls.Add(this.BUILD_COMPUTE);
            this.Build.Controls.Add(this.BUILD_REDIST);
            this.Build.Location = new System.Drawing.Point(349, 72);
            this.Build.Name = "Build";
            this.Build.Size = new System.Drawing.Size(156, 116);
            this.Build.TabIndex = 5;
            this.Build.TabStop = false;
            this.Build.Text = "Build";
            // 
            // BUILD_DEBUG
            // 
            this.BUILD_DEBUG.AutoSize = true;
            this.BUILD_DEBUG.Location = new System.Drawing.Point(7, 68);
            this.BUILD_DEBUG.Name = "BUILD_DEBUG";
            this.BUILD_DEBUG.Size = new System.Drawing.Size(85, 17);
            this.BUILD_DEBUG.TabIndex = 2;
            this.BUILD_DEBUG.Text = "DEBUG Info";
            this.BUILD_DEBUG.UseVisualStyleBackColor = true;
            // 
            // BUILD_COMPUTE
            // 
            this.BUILD_COMPUTE.AutoSize = true;
            this.BUILD_COMPUTE.Location = new System.Drawing.Point(7, 44);
            this.BUILD_COMPUTE.Name = "BUILD_COMPUTE";
            this.BUILD_COMPUTE.Size = new System.Drawing.Size(102, 17);
            this.BUILD_COMPUTE.TabIndex = 1;
            this.BUILD_COMPUTE.Text = "GPU Computing";
            this.BUILD_COMPUTE.UseVisualStyleBackColor = true;
            // 
            // BUILD_REDIST
            // 
            this.BUILD_REDIST.AutoSize = true;
            this.BUILD_REDIST.Location = new System.Drawing.Point(7, 20);
            this.BUILD_REDIST.Name = "BUILD_REDIST";
            this.BUILD_REDIST.Size = new System.Drawing.Size(98, 17);
            this.BUILD_REDIST.TabIndex = 0;
            this.BUILD_REDIST.Text = "Documentation";
            this.BUILD_REDIST.UseVisualStyleBackColor = true;
            // 
            // CPU
            // 
            this.CPU.Controls.Add(this.SSE2);
            this.CPU.Controls.Add(this.SSE);
            this.CPU.Controls.Add(this.SMP);
            this.CPU.Location = new System.Drawing.Point(349, 208);
            this.CPU.Name = "CPU";
            this.CPU.Size = new System.Drawing.Size(156, 40);
            this.CPU.TabIndex = 6;
            this.CPU.TabStop = false;
            this.CPU.Text = "CPU";
            // 
            // SSE2
            // 
            this.SSE2.AutoSize = true;
            this.SSE2.Location = new System.Drawing.Point(101, 20);
            this.SSE2.Name = "SSE2";
            this.SSE2.Size = new System.Drawing.Size(53, 17);
            this.SSE2.TabIndex = 2;
            this.SSE2.Text = "SSE2";
            this.SSE2.UseVisualStyleBackColor = true;
            // 
            // SSE
            // 
            this.SSE.AutoSize = true;
            this.SSE.Location = new System.Drawing.Point(55, 20);
            this.SSE.Name = "SSE";
            this.SSE.Size = new System.Drawing.Size(47, 17);
            this.SSE.TabIndex = 1;
            this.SSE.Text = "SSE";
            this.SSE.UseVisualStyleBackColor = true;
            // 
            // SMP
            // 
            this.SMP.AutoSize = true;
            this.SMP.Location = new System.Drawing.Point(7, 20);
            this.SMP.Name = "SMP";
            this.SMP.Size = new System.Drawing.Size(49, 17);
            this.SMP.TabIndex = 0;
            this.SMP.Text = "SMP";
            this.SMP.UseVisualStyleBackColor = true;
            // 
            // TREE_EXTENSIONS
            // 
            this.TREE_EXTENSIONS.CheckBoxes = true;
            this.TREE_EXTENSIONS.Location = new System.Drawing.Point(13, 13);
            this.TREE_EXTENSIONS.Name = "TREE_EXTENSIONS";
            this.TREE_EXTENSIONS.Size = new System.Drawing.Size(330, 306);
            this.TREE_EXTENSIONS.TabIndex = 0;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBox1.Image = global::Builder.Properties.Resources.Raptor_logo_sml;
            this.pictureBox1.Location = new System.Drawing.Point(349, 254);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(154, 65);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 8;
            this.pictureBox1.TabStop = false;
            // 
            // BUILD_PHYSX
            // 
            this.BUILD_PHYSX.AutoSize = true;
            this.BUILD_PHYSX.Location = new System.Drawing.Point(7, 91);
            this.BUILD_PHYSX.Name = "BUILD_PHYSX";
            this.BUILD_PHYSX.Size = new System.Drawing.Size(84, 17);
            this.BUILD_PHYSX.TabIndex = 3;
            this.BUILD_PHYSX.Text = "Cuda PhysX";
            this.BUILD_PHYSX.UseVisualStyleBackColor = true;
            // 
            // BuilderForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(519, 331);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.TREE_EXTENSIONS);
            this.Controls.Add(this.CPU);
            this.Controls.Add(this.Build);
            this.Controls.Add(this.SelectNone);
            this.Controls.Add(this.SelectAll);
            this.Controls.Add(this.Cancel);
            this.Controls.Add(this.OK);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "BuilderForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "Raptor Builder.Net  [Prepare Raptor Code to use the following options] ";
            this.Build.ResumeLayout(false);
            this.Build.PerformLayout();
            this.CPU.ResumeLayout(false);
            this.CPU.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button OK;
        private System.Windows.Forms.Button Cancel;
        private System.Windows.Forms.Button SelectAll;
        private System.Windows.Forms.Button SelectNone;
        private System.Windows.Forms.GroupBox Build;
        private System.Windows.Forms.GroupBox CPU;
        private System.Windows.Forms.CheckBox BUILD_DEBUG;
        private System.Windows.Forms.CheckBox BUILD_COMPUTE;
        private System.Windows.Forms.CheckBox BUILD_REDIST;
        private System.Windows.Forms.CheckBox SSE2;
        private System.Windows.Forms.CheckBox SSE;
        private System.Windows.Forms.CheckBox SMP;
        private System.Windows.Forms.TreeView TREE_EXTENSIONS;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.CheckBox BUILD_PHYSX;
    }
}

