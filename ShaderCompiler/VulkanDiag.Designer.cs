namespace ShaderCompiler
{
    partial class VulkanDiag
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.PictureBox pictureBox2;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(VulkanDiag));
            System.Windows.Forms.PictureBox pictureBox1;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label1;
            System.Windows.Forms.GroupBox groupBox1;
            System.Windows.Forms.GroupBox groupBox2;
            System.Windows.Forms.GroupBox groupBox3;
            this.SPIRV = new System.Windows.Forms.Label();
            this.Version = new System.Windows.Forms.Label();
            this.Renderer = new System.Windows.Forms.Label();
            this.Vendor = new System.Windows.Forms.Label();
            this.VKLimits = new System.Windows.Forms.Panel();
            this.CloseButton = new System.Windows.Forms.Button();
            this.VKExtensions = new System.Windows.Forms.ListBox();
            pictureBox2 = new System.Windows.Forms.PictureBox();
            pictureBox1 = new System.Windows.Forms.PictureBox();
            label4 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label1 = new System.Windows.Forms.Label();
            groupBox1 = new System.Windows.Forms.GroupBox();
            groupBox2 = new System.Windows.Forms.GroupBox();
            groupBox3 = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(pictureBox2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(pictureBox1)).BeginInit();
            groupBox1.SuspendLayout();
            groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox2
            // 
            pictureBox2.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox2.Image")));
            pictureBox2.Location = new System.Drawing.Point(146, 12);
            pictureBox2.Name = "pictureBox2";
            pictureBox2.Size = new System.Drawing.Size(120, 50);
            pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            pictureBox2.TabIndex = 4;
            pictureBox2.TabStop = false;
            // 
            // pictureBox1
            // 
            pictureBox1.Image = global::ShaderCompiler.Properties.Resources.Raptor_logo_sml;
            pictureBox1.Location = new System.Drawing.Point(12, 13);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new System.Drawing.Size(128, 46);
            pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            pictureBox1.TabIndex = 2;
            pictureBox1.TabStop = false;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(7, 60);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(35, 13);
            label4.TabIndex = 3;
            label4.Text = "SpirV:";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(7, 47);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(45, 13);
            label3.TabIndex = 2;
            label3.Text = "Version:";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(7, 34);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(54, 13);
            label2.TabIndex = 1;
            label2.Text = "Renderer:";
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(7, 21);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(44, 13);
            label1.TabIndex = 0;
            label1.Text = "Vendor:";
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(this.SPIRV);
            groupBox1.Controls.Add(this.Version);
            groupBox1.Controls.Add(this.Renderer);
            groupBox1.Controls.Add(this.Vendor);
            groupBox1.Controls.Add(label4);
            groupBox1.Controls.Add(label3);
            groupBox1.Controls.Add(label2);
            groupBox1.Controls.Add(label1);
            groupBox1.Location = new System.Drawing.Point(12, 68);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new System.Drawing.Size(337, 80);
            groupBox1.TabIndex = 5;
            groupBox1.TabStop = false;
            groupBox1.Text = "Vulkan Engine";
            // 
            // SPIRV
            // 
            this.SPIRV.AutoSize = true;
            this.SPIRV.Location = new System.Drawing.Point(67, 60);
            this.SPIRV.Name = "SPIRV";
            this.SPIRV.Size = new System.Drawing.Size(10, 13);
            this.SPIRV.TabIndex = 7;
            this.SPIRV.Text = "I";
            // 
            // Version
            // 
            this.Version.AutoSize = true;
            this.Version.Location = new System.Drawing.Point(67, 47);
            this.Version.Name = "Version";
            this.Version.Size = new System.Drawing.Size(10, 13);
            this.Version.TabIndex = 6;
            this.Version.Text = "I";
            // 
            // Renderer
            // 
            this.Renderer.AutoSize = true;
            this.Renderer.Location = new System.Drawing.Point(67, 34);
            this.Renderer.Name = "Renderer";
            this.Renderer.Size = new System.Drawing.Size(10, 13);
            this.Renderer.TabIndex = 5;
            this.Renderer.Text = "I";
            // 
            // Vendor
            // 
            this.Vendor.AutoSize = true;
            this.Vendor.Location = new System.Drawing.Point(67, 21);
            this.Vendor.Name = "Vendor";
            this.Vendor.Size = new System.Drawing.Size(10, 13);
            this.Vendor.TabIndex = 4;
            this.Vendor.Text = "I";
            // 
            // groupBox2
            // 
            groupBox2.Controls.Add(this.VKLimits);
            groupBox2.Location = new System.Drawing.Point(12, 154);
            groupBox2.Name = "groupBox2";
            groupBox2.Size = new System.Drawing.Size(336, 124);
            groupBox2.TabIndex = 6;
            groupBox2.TabStop = false;
            groupBox2.Text = "Vulkan Limits";
            // 
            // VKLimits
            // 
            this.VKLimits.AutoScroll = true;
            this.VKLimits.Dock = System.Windows.Forms.DockStyle.Fill;
            this.VKLimits.Location = new System.Drawing.Point(3, 16);
            this.VKLimits.Name = "VKLimits";
            this.VKLimits.Size = new System.Drawing.Size(330, 105);
            this.VKLimits.TabIndex = 0;
            // 
            // Close
            // 
            this.CloseButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.CloseButton.Location = new System.Drawing.Point(272, 13);
            this.CloseButton.Name = "Close";
            this.CloseButton.Size = new System.Drawing.Size(75, 23);
            this.CloseButton.TabIndex = 0;
            this.CloseButton.Text = "Close";
            this.CloseButton.UseVisualStyleBackColor = true;
            this.CloseButton.Click += new System.EventHandler(this.onClose);
            // 
            // VKExtensions
            // 
            this.VKExtensions.FormattingEnabled = true;
            this.VKExtensions.Location = new System.Drawing.Point(18, 306);
            this.VKExtensions.Name = "VKExtensions";
            this.VKExtensions.Size = new System.Drawing.Size(324, 121);
            this.VKExtensions.TabIndex = 0;
            // 
            // groupBox3
            // 
            groupBox3.Location = new System.Drawing.Point(12, 284);
            groupBox3.Name = "groupBox3";
            groupBox3.Size = new System.Drawing.Size(337, 156);
            groupBox3.TabIndex = 7;
            groupBox3.TabStop = false;
            groupBox3.Text = "Vulkan Extensions";
            // 
            // VulkanDiag
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(357, 450);
            this.Controls.Add(this.VKExtensions);
            this.Controls.Add(groupBox3);
            this.Controls.Add(groupBox2);
            this.Controls.Add(groupBox1);
            this.Controls.Add(pictureBox2);
            this.Controls.Add(pictureBox1);
            this.Controls.Add(this.CloseButton);
            this.Name = "VulkanDiag";
            this.Text = "VulkanDiag";
            ((System.ComponentModel.ISupportInitialize)(pictureBox2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(pictureBox1)).EndInit();
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button CloseButton;
        public System.Windows.Forms.Label Vendor;
        public System.Windows.Forms.Label SPIRV;
        public System.Windows.Forms.Label Version;
        public System.Windows.Forms.Label Renderer;
        public System.Windows.Forms.Panel VKLimits;
        public System.Windows.Forms.ListBox VKExtensions;
    }
}