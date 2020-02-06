﻿namespace RaysClient
{
    partial class RaysClientForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RaysClientForm));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.port = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.host = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.Connect = new System.Windows.Forms.Button();
            this.Open = new System.Windows.Forms.Button();
            this.Log = new System.Windows.Forms.ListBox();
            this.Quit = new System.Windows.Forms.Button();
            this.Scene = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.Assets = new System.Windows.Forms.ListBox();
            this.Render = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.port);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.host);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.Connect);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(311, 46);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Server connection";
            // 
            // port
            // 
            this.port.Location = new System.Drawing.Point(161, 16);
            this.port.Name = "port";
            this.port.Size = new System.Drawing.Size(54, 20);
            this.port.TabIndex = 4;
            this.port.Text = "0";
            this.port.TextChanged += new System.EventHandler(this.onPortChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(133, 20);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Port:";
            // 
            // host
            // 
            this.host.Location = new System.Drawing.Point(26, 16);
            this.host.Name = "host";
            this.host.Size = new System.Drawing.Size(100, 20);
            this.host.TabIndex = 2;
            this.host.Text = "0.0.0.0";
            this.host.TextChanged += new System.EventHandler(this.onIPChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(20, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "IP:";
            // 
            // Connect
            // 
            this.Connect.Enabled = false;
            this.Connect.Location = new System.Drawing.Point(229, 13);
            this.Connect.Name = "Connect";
            this.Connect.Size = new System.Drawing.Size(75, 23);
            this.Connect.TabIndex = 0;
            this.Connect.Text = "Connect";
            this.Connect.UseVisualStyleBackColor = true;
            this.Connect.Click += new System.EventHandler(this.onConnect);
            // 
            // Open
            // 
            this.Open.Location = new System.Drawing.Point(241, 64);
            this.Open.Name = "Open";
            this.Open.Size = new System.Drawing.Size(75, 23);
            this.Open.TabIndex = 6;
            this.Open.Text = "Open...";
            this.Open.UseVisualStyleBackColor = true;
            this.Open.Click += new System.EventHandler(this.onOpen);
            // 
            // Log
            // 
            this.Log.FormattingEnabled = true;
            this.Log.HorizontalScrollbar = true;
            this.Log.Location = new System.Drawing.Point(12, 181);
            this.Log.Name = "Log";
            this.Log.Size = new System.Drawing.Size(311, 95);
            this.Log.TabIndex = 5;
            // 
            // Quit
            // 
            this.Quit.Location = new System.Drawing.Point(241, 140);
            this.Quit.Name = "Quit";
            this.Quit.Size = new System.Drawing.Size(75, 23);
            this.Quit.TabIndex = 4;
            this.Quit.Text = "Quit";
            this.Quit.UseVisualStyleBackColor = true;
            this.Quit.Click += new System.EventHandler(this.OnQuit);
            // 
            // Scene
            // 
            this.Scene.AutoSize = true;
            this.Scene.Location = new System.Drawing.Point(7, 16);
            this.Scene.Name = "Scene";
            this.Scene.Size = new System.Drawing.Size(41, 13);
            this.Scene.TabIndex = 7;
            this.Scene.Text = "Scene:";
            this.Scene.TextChanged += new System.EventHandler(this.onSceneChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.Assets);
            this.groupBox2.Controls.Add(this.Scene);
            this.groupBox2.Location = new System.Drawing.Point(12, 63);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(223, 100);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Assets";
            // 
            // Assets
            // 
            this.Assets.FormattingEnabled = true;
            this.Assets.Location = new System.Drawing.Point(10, 33);
            this.Assets.Name = "Assets";
            this.Assets.Size = new System.Drawing.Size(205, 56);
            this.Assets.TabIndex = 8;
            // 
            // Render
            // 
            this.Render.Location = new System.Drawing.Point(242, 94);
            this.Render.Name = "Render";
            this.Render.Size = new System.Drawing.Size(75, 23);
            this.Render.TabIndex = 9;
            this.Render.Text = "Render";
            this.Render.UseVisualStyleBackColor = true;
            this.Render.Click += new System.EventHandler(this.onRender);
            // 
            // RaysClientForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ClientSize = new System.Drawing.Size(335, 283);
            this.Controls.Add(this.Render);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.Open);
            this.Controls.Add(this.Log);
            this.Controls.Add(this.Quit);
            this.Controls.Add(this.groupBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "RaysClientForm";
            this.Text = "Rays Client";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.onClose);
            this.Load += new System.EventHandler(this.RaysClientForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox port;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox host;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button Connect;
        private System.Windows.Forms.Button Open;
        private System.Windows.Forms.ListBox Log;
        private System.Windows.Forms.Button Quit;
        private System.Windows.Forms.Label Scene;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ListBox Assets;
        private System.Windows.Forms.Button Render;
    }
}

