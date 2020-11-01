namespace RaysClient
{
    partial class RaysConfig
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
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label5;
            System.Windows.Forms.Label label6;
            System.Windows.Forms.Label label7;
            System.Windows.Forms.Label label8;
            System.Windows.Forms.Label label9;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RaysConfig));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ObjectPlane = new System.Windows.Forms.TextBox();
            this.RenderingHeight = new System.Windows.Forms.TextBox();
            this.RenderingWidth = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.Aperture = new System.Windows.Forms.TextBox();
            this.Focale = new System.Windows.Forms.TextBox();
            this.Crease = new System.Windows.Forms.TextBox();
            this.Variance = new System.Windows.Forms.TextBox();
            this.Refraction = new System.Windows.Forms.TextBox();
            this.Reflection = new System.Windows.Forms.TextBox();
            this.Apply = new System.Windows.Forms.Button();
            this.Cancel = new System.Windows.Forms.Button();
            label3 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            label5 = new System.Windows.Forms.Label();
            label6 = new System.Windows.Forms.Label();
            label7 = new System.Windows.Forms.Label();
            label8 = new System.Windows.Forms.Label();
            label9 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(6, 16);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(88, 13);
            label3.TabIndex = 2;
            label3.Text = "Reflection depth:";
            label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(6, 42);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(89, 13);
            label4.TabIndex = 3;
            label4.Text = "Refraction depth:";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new System.Drawing.Point(28, 68);
            label5.Name = "label5";
            label5.Size = new System.Drawing.Size(52, 13);
            label5.TabIndex = 4;
            label5.Text = "Variance:";
            label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Location = new System.Drawing.Point(8, 95);
            label6.Name = "label6";
            label6.Size = new System.Drawing.Size(72, 13);
            label6.TabIndex = 7;
            label6.Text = "Crease angle:";
            label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label7
            // 
            label7.AutoSize = true;
            label7.Location = new System.Drawing.Point(159, 69);
            label7.Name = "label7";
            label7.Size = new System.Drawing.Size(42, 13);
            label7.TabIndex = 4;
            label7.Text = "Focale:";
            label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label8
            // 
            label8.AutoSize = true;
            label8.Location = new System.Drawing.Point(23, 69);
            label8.Name = "label8";
            label8.Size = new System.Drawing.Size(70, 13);
            label8.TabIndex = 5;
            label8.Text = "Object plane:";
            label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label9
            // 
            label9.AutoSize = true;
            label9.Location = new System.Drawing.Point(151, 95);
            label9.Name = "label9";
            label9.Size = new System.Drawing.Size(50, 13);
            label9.TabIndex = 8;
            label9.Text = "Aperture:";
            label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(87, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Rendering width:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(2, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(91, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Rendering height:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(label8);
            this.groupBox1.Controls.Add(this.ObjectPlane);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.RenderingHeight);
            this.groupBox1.Controls.Add(this.RenderingWidth);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Location = new System.Drawing.Point(12, 10);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(175, 100);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Viewport";
            // 
            // ObjectPlane
            // 
            this.ObjectPlane.Location = new System.Drawing.Point(99, 66);
            this.ObjectPlane.Name = "ObjectPlane";
            this.ObjectPlane.Size = new System.Drawing.Size(53, 20);
            this.ObjectPlane.TabIndex = 4;
            this.ObjectPlane.Text = "5.0";
            this.ObjectPlane.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ObjectPlane.TextChanged += new System.EventHandler(this.onPlane);
            // 
            // RenderingHeight
            // 
            this.RenderingHeight.Location = new System.Drawing.Point(99, 39);
            this.RenderingHeight.Name = "RenderingHeight";
            this.RenderingHeight.Size = new System.Drawing.Size(53, 20);
            this.RenderingHeight.TabIndex = 3;
            this.RenderingHeight.Text = "768";
            this.RenderingHeight.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.RenderingHeight.TextChanged += new System.EventHandler(this.onHeight);
            // 
            // RenderingWidth
            // 
            this.RenderingWidth.Location = new System.Drawing.Point(99, 13);
            this.RenderingWidth.Name = "RenderingWidth";
            this.RenderingWidth.Size = new System.Drawing.Size(53, 20);
            this.RenderingWidth.TabIndex = 0;
            this.RenderingWidth.Text = "1024";
            this.RenderingWidth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.RenderingWidth.TextChanged += new System.EventHandler(this.onWidth);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(label9);
            this.groupBox2.Controls.Add(this.Aperture);
            this.groupBox2.Controls.Add(label6);
            this.groupBox2.Controls.Add(this.Focale);
            this.groupBox2.Controls.Add(this.Crease);
            this.groupBox2.Controls.Add(label7);
            this.groupBox2.Controls.Add(this.Variance);
            this.groupBox2.Controls.Add(label5);
            this.groupBox2.Controls.Add(label4);
            this.groupBox2.Controls.Add(label3);
            this.groupBox2.Controls.Add(this.Refraction);
            this.groupBox2.Controls.Add(this.Reflection);
            this.groupBox2.Location = new System.Drawing.Point(12, 116);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(262, 128);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Rays casting";
            // 
            // Aperture
            // 
            this.Aperture.Location = new System.Drawing.Point(207, 92);
            this.Aperture.Name = "Aperture";
            this.Aperture.Size = new System.Drawing.Size(49, 20);
            this.Aperture.TabIndex = 6;
            this.Aperture.Text = "2.0";
            this.Aperture.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.Aperture.TextChanged += new System.EventHandler(this.onAperture);
            // 
            // Focale
            // 
            this.Focale.Location = new System.Drawing.Point(207, 66);
            this.Focale.Name = "Focale";
            this.Focale.Size = new System.Drawing.Size(49, 20);
            this.Focale.TabIndex = 5;
            this.Focale.Text = "20.0";
            this.Focale.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.Focale.TextChanged += new System.EventHandler(this.onFocale);
            // 
            // Crease
            // 
            this.Crease.Location = new System.Drawing.Point(87, 92);
            this.Crease.Name = "Crease";
            this.Crease.Size = new System.Drawing.Size(53, 20);
            this.Crease.TabIndex = 6;
            this.Crease.Text = "0.0";
            this.Crease.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.Crease.TextChanged += new System.EventHandler(this.onCrease);
            // 
            // Variance
            // 
            this.Variance.Location = new System.Drawing.Point(87, 65);
            this.Variance.Name = "Variance";
            this.Variance.Size = new System.Drawing.Size(53, 20);
            this.Variance.TabIndex = 5;
            this.Variance.Text = "0";
            this.Variance.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.Variance.TextChanged += new System.EventHandler(this.onVariance);
            // 
            // Refraction
            // 
            this.Refraction.Location = new System.Drawing.Point(101, 39);
            this.Refraction.Name = "Refraction";
            this.Refraction.Size = new System.Drawing.Size(53, 20);
            this.Refraction.TabIndex = 1;
            this.Refraction.Text = "1";
            this.Refraction.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.Refraction.TextChanged += new System.EventHandler(this.onRefraction);
            // 
            // Reflection
            // 
            this.Reflection.Location = new System.Drawing.Point(100, 13);
            this.Reflection.Name = "Reflection";
            this.Reflection.Size = new System.Drawing.Size(54, 20);
            this.Reflection.TabIndex = 0;
            this.Reflection.Text = "1";
            this.Reflection.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.Reflection.TextChanged += new System.EventHandler(this.onReflection);
            // 
            // Apply
            // 
            this.Apply.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.Apply.Location = new System.Drawing.Point(199, 19);
            this.Apply.Name = "Apply";
            this.Apply.Size = new System.Drawing.Size(75, 23);
            this.Apply.TabIndex = 5;
            this.Apply.Text = "Apply";
            this.Apply.UseVisualStyleBackColor = true;
            this.Apply.Click += new System.EventHandler(this.onApply);
            // 
            // Cancel
            // 
            this.Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Cancel.Location = new System.Drawing.Point(199, 48);
            this.Cancel.Name = "Cancel";
            this.Cancel.Size = new System.Drawing.Size(75, 23);
            this.Cancel.TabIndex = 6;
            this.Cancel.Text = "Cancel";
            this.Cancel.UseVisualStyleBackColor = true;
            this.Cancel.Click += new System.EventHandler(this.onCancel);
            // 
            // RaysConfig
            // 
            this.AcceptButton = this.Apply;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.Cancel;
            this.ClientSize = new System.Drawing.Size(286, 250);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.Cancel);
            this.Controls.Add(this.Apply);
            this.Controls.Add(this.groupBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "RaysConfig";
            this.Text = "Rays Configuration";
            this.Load += new System.EventHandler(this.onLoad);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox RenderingWidth;
        private System.Windows.Forms.TextBox RenderingHeight;
        private System.Windows.Forms.Button Apply;
        private System.Windows.Forms.Button Cancel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox Refraction;
        private System.Windows.Forms.TextBox Reflection;
        private System.Windows.Forms.TextBox Variance;
        private System.Windows.Forms.TextBox Crease;
        private System.Windows.Forms.TextBox Focale;
        private System.Windows.Forms.TextBox ObjectPlane;
        private System.Windows.Forms.TextBox Aperture;
    }
}