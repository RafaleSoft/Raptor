﻿namespace ShaderCompiler
{
    partial class ShaderCompiler
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.GroupBox groupBox1;
            System.Windows.Forms.GroupBox groupBox2;
            this.FShaderName = new System.Windows.Forms.Label();
            this.TShaderName = new System.Windows.Forms.Label();
            this.GShaderName = new System.Windows.Forms.Label();
            this.VShaderName = new System.Windows.Forms.Label();
            this.Fragment = new System.Windows.Forms.Button();
            this.Tesselate = new System.Windows.Forms.Button();
            this.Geometry = new System.Windows.Forms.Button();
            this.Vertex = new System.Windows.Forms.Button();
            this.Quit = new System.Windows.Forms.Button();
            this.Canvas = new System.Windows.Forms.PictureBox();
            this.renderTimer = new System.Windows.Forms.Timer(this.components);
            groupBox1 = new System.Windows.Forms.GroupBox();
            groupBox2 = new System.Windows.Forms.GroupBox();
            groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Canvas)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(this.FShaderName);
            groupBox1.Controls.Add(this.TShaderName);
            groupBox1.Controls.Add(this.GShaderName);
            groupBox1.Controls.Add(this.VShaderName);
            groupBox1.Controls.Add(this.Fragment);
            groupBox1.Controls.Add(this.Tesselate);
            groupBox1.Controls.Add(this.Geometry);
            groupBox1.Controls.Add(this.Vertex);
            groupBox1.Location = new System.Drawing.Point(660, 42);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new System.Drawing.Size(185, 200);
            groupBox1.TabIndex = 3;
            groupBox1.TabStop = false;
            groupBox1.Text = "Shaders";
            // 
            // FShaderName
            // 
            this.FShaderName.AutoSize = true;
            this.FShaderName.Location = new System.Drawing.Point(87, 111);
            this.FShaderName.Name = "FShaderName";
            this.FShaderName.Size = new System.Drawing.Size(33, 13);
            this.FShaderName.TabIndex = 8;
            this.FShaderName.Text = "None";
            // 
            // TShaderName
            // 
            this.TShaderName.AutoSize = true;
            this.TShaderName.Location = new System.Drawing.Point(87, 82);
            this.TShaderName.Name = "TShaderName";
            this.TShaderName.Size = new System.Drawing.Size(33, 13);
            this.TShaderName.TabIndex = 7;
            this.TShaderName.Text = "None";
            // 
            // GShaderName
            // 
            this.GShaderName.AutoSize = true;
            this.GShaderName.Location = new System.Drawing.Point(87, 53);
            this.GShaderName.Name = "GShaderName";
            this.GShaderName.Size = new System.Drawing.Size(33, 13);
            this.GShaderName.TabIndex = 6;
            this.GShaderName.Text = "None";
            // 
            // VShaderName
            // 
            this.VShaderName.AutoSize = true;
            this.VShaderName.Location = new System.Drawing.Point(87, 24);
            this.VShaderName.Name = "VShaderName";
            this.VShaderName.Size = new System.Drawing.Size(33, 13);
            this.VShaderName.TabIndex = 5;
            this.VShaderName.Text = "None";
            // 
            // Fragment
            // 
            this.Fragment.Location = new System.Drawing.Point(6, 106);
            this.Fragment.Name = "Fragment";
            this.Fragment.Size = new System.Drawing.Size(75, 23);
            this.Fragment.TabIndex = 4;
            this.Fragment.Text = "Fragment ...";
            this.Fragment.UseVisualStyleBackColor = true;
            this.Fragment.Click += new System.EventHandler(this.onFragment);
            // 
            // Tesselate
            // 
            this.Tesselate.Enabled = false;
            this.Tesselate.Location = new System.Drawing.Point(6, 77);
            this.Tesselate.Name = "Tesselate";
            this.Tesselate.Size = new System.Drawing.Size(75, 23);
            this.Tesselate.TabIndex = 3;
            this.Tesselate.Text = "Tesselate ...";
            this.Tesselate.UseVisualStyleBackColor = true;
            this.Tesselate.Click += new System.EventHandler(this.onTesselate);
            // 
            // Geometry
            // 
            this.Geometry.Location = new System.Drawing.Point(6, 48);
            this.Geometry.Name = "Geometry";
            this.Geometry.Size = new System.Drawing.Size(75, 23);
            this.Geometry.TabIndex = 2;
            this.Geometry.Text = "Geometry ...";
            this.Geometry.UseVisualStyleBackColor = true;
            this.Geometry.Click += new System.EventHandler(this.onGeometry);
            // 
            // Vertex
            // 
            this.Vertex.Location = new System.Drawing.Point(6, 19);
            this.Vertex.Name = "Vertex";
            this.Vertex.Size = new System.Drawing.Size(75, 23);
            this.Vertex.TabIndex = 1;
            this.Vertex.Text = "Vertex ...";
            this.Vertex.UseVisualStyleBackColor = true;
            this.Vertex.Click += new System.EventHandler(this.onVertex);
            // 
            // groupBox2
            // 
            groupBox2.Location = new System.Drawing.Point(660, 248);
            groupBox2.Name = "groupBox2";
            groupBox2.Size = new System.Drawing.Size(185, 245);
            groupBox2.TabIndex = 4;
            groupBox2.TabStop = false;
            groupBox2.Text = "Assets";
            // 
            // Quit
            // 
            this.Quit.Location = new System.Drawing.Point(770, 13);
            this.Quit.Name = "Quit";
            this.Quit.Size = new System.Drawing.Size(75, 23);
            this.Quit.TabIndex = 0;
            this.Quit.Text = "Quit";
            this.Quit.UseVisualStyleBackColor = true;
            this.Quit.Click += new System.EventHandler(this.onQuit);
            // 
            // Canvas
            // 
            this.Canvas.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.Canvas.Location = new System.Drawing.Point(13, 13);
            this.Canvas.Name = "Canvas";
            this.Canvas.Size = new System.Drawing.Size(640, 480);
            this.Canvas.TabIndex = 2;
            this.Canvas.TabStop = false;
            this.Canvas.Paint += new System.Windows.Forms.PaintEventHandler(this.onPaint);
            // 
            // renderTimer
            // 
            this.renderTimer.Interval = 10;
            this.renderTimer.Tick += new System.EventHandler(this.onTick);
            // 
            // ShaderCompiler
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(857, 505);
            this.Controls.Add(groupBox2);
            this.Controls.Add(this.Canvas);
            this.Controls.Add(this.Quit);
            this.Controls.Add(groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MinimizeBox = false;
            this.Name = "ShaderCompiler";
            this.Text = "Raptor Shader Compiler";
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Canvas)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button Quit;
        private System.Windows.Forms.Button Vertex;
        private System.Windows.Forms.PictureBox Canvas;
        private System.Windows.Forms.Button Geometry;
        private System.Windows.Forms.Button Tesselate;
        private System.Windows.Forms.Button Fragment;
        private System.Windows.Forms.Label VShaderName;
        private System.Windows.Forms.Label GShaderName;
        private System.Windows.Forms.Label TShaderName;
        private System.Windows.Forms.Label FShaderName;
        private System.Windows.Forms.Timer renderTimer;
    }
}

