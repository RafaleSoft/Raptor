namespace RaysClient
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
            System.Windows.Forms.GroupBox groupBox1;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label1;
            System.Windows.Forms.GroupBox groupBox2;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.GroupBox groupBox3;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RaysClientForm));
            this.port = new System.Windows.Forms.TextBox();
            this.host = new System.Windows.Forms.TextBox();
            this.Connect = new System.Windows.Forms.Button();
            this.Assets = new System.Windows.Forms.ListBox();
            this.Scene = new System.Windows.Forms.Label();
            this.Render = new System.Windows.Forms.Button();
            this.Open = new System.Windows.Forms.Button();
            this.Configuration = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.SaveAs = new System.Windows.Forms.Button();
            this.Resume = new System.Windows.Forms.Button();
            this.Quit = new System.Windows.Forms.Button();
            this.Log = new System.Windows.Forms.ListBox();
            groupBox1 = new System.Windows.Forms.GroupBox();
            label2 = new System.Windows.Forms.Label();
            label1 = new System.Windows.Forms.Label();
            groupBox2 = new System.Windows.Forms.GroupBox();
            label3 = new System.Windows.Forms.Label();
            groupBox3 = new System.Windows.Forms.GroupBox();
            groupBox1.SuspendLayout();
            groupBox2.SuspendLayout();
            groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(this.port);
            groupBox1.Controls.Add(label2);
            groupBox1.Controls.Add(this.host);
            groupBox1.Controls.Add(label1);
            groupBox1.Controls.Add(this.Connect);
            groupBox1.Location = new System.Drawing.Point(12, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new System.Drawing.Size(311, 46);
            groupBox1.TabIndex = 1;
            groupBox1.TabStop = false;
            groupBox1.Text = "Server connection";
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
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(133, 20);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(29, 13);
            label2.TabIndex = 3;
            label2.Text = "Port:";
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
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(7, 20);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(20, 13);
            label1.TabIndex = 1;
            label1.Text = "IP:";
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
            // groupBox2
            // 
            groupBox2.Controls.Add(this.Assets);
            groupBox2.Controls.Add(this.Scene);
            groupBox2.Controls.Add(this.Render);
            groupBox2.Controls.Add(this.Open);
            groupBox2.Controls.Add(this.Configuration);
            groupBox2.Location = new System.Drawing.Point(12, 63);
            groupBox2.Name = "groupBox2";
            groupBox2.Size = new System.Drawing.Size(311, 100);
            groupBox2.TabIndex = 8;
            groupBox2.TabStop = false;
            groupBox2.Text = "Assets";
            // 
            // Assets
            // 
            this.Assets.FormattingEnabled = true;
            this.Assets.Location = new System.Drawing.Point(10, 33);
            this.Assets.Name = "Assets";
            this.Assets.Size = new System.Drawing.Size(205, 56);
            this.Assets.TabIndex = 8;
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
            // Render
            // 
            this.Render.Location = new System.Drawing.Point(229, 71);
            this.Render.Name = "Render";
            this.Render.Size = new System.Drawing.Size(75, 23);
            this.Render.TabIndex = 9;
            this.Render.Text = "Render";
            this.Render.UseVisualStyleBackColor = true;
            this.Render.Click += new System.EventHandler(this.onRender);
            // 
            // Open
            // 
            this.Open.Location = new System.Drawing.Point(229, 13);
            this.Open.Name = "Open";
            this.Open.Size = new System.Drawing.Size(75, 23);
            this.Open.TabIndex = 6;
            this.Open.Text = "Open...";
            this.Open.UseVisualStyleBackColor = true;
            this.Open.Click += new System.EventHandler(this.onOpen);
            // 
            // Configuration
            // 
            this.Configuration.Location = new System.Drawing.Point(229, 42);
            this.Configuration.Name = "Configuration";
            this.Configuration.Size = new System.Drawing.Size(75, 23);
            this.Configuration.TabIndex = 10;
            this.Configuration.Text = "Configure...";
            this.Configuration.UseVisualStyleBackColor = true;
            this.Configuration.Click += new System.EventHandler(this.onConfig);
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(7, 36);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(51, 13);
            label3.TabIndex = 12;
            label3.Text = "Progress:";
            // 
            // groupBox3
            // 
            groupBox3.Controls.Add(this.progressBar1);
            groupBox3.Controls.Add(label3);
            groupBox3.Controls.Add(this.SaveAs);
            groupBox3.Controls.Add(this.Resume);
            groupBox3.Controls.Add(this.Quit);
            groupBox3.Location = new System.Drawing.Point(12, 170);
            groupBox3.Name = "groupBox3";
            groupBox3.Size = new System.Drawing.Size(311, 169);
            groupBox3.TabIndex = 14;
            groupBox3.TabStop = false;
            groupBox3.Text = "Rendering status";
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(10, 52);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(294, 11);
            this.progressBar1.TabIndex = 11;
            // 
            // SaveAs
            // 
            this.SaveAs.Location = new System.Drawing.Point(148, 23);
            this.SaveAs.Name = "SaveAs";
            this.SaveAs.Size = new System.Drawing.Size(75, 23);
            this.SaveAs.TabIndex = 13;
            this.SaveAs.Text = "SaveAs...";
            this.SaveAs.UseVisualStyleBackColor = true;
            this.SaveAs.Click += new System.EventHandler(this.onSaveAs);
            // 
            // Resume
            // 
            this.Resume.Location = new System.Drawing.Point(67, 23);
            this.Resume.Name = "Resume";
            this.Resume.Size = new System.Drawing.Size(75, 23);
            this.Resume.TabIndex = 0;
            this.Resume.Text = "Resume";
            this.Resume.UseVisualStyleBackColor = true;
            this.Resume.Click += new System.EventHandler(this.onResume);
            // 
            // Quit
            // 
            this.Quit.Location = new System.Drawing.Point(229, 23);
            this.Quit.Name = "Quit";
            this.Quit.Size = new System.Drawing.Size(75, 23);
            this.Quit.TabIndex = 4;
            this.Quit.Text = "Quit";
            this.Quit.UseVisualStyleBackColor = true;
            this.Quit.Click += new System.EventHandler(this.OnQuit);
            // 
            // Log
            // 
            this.Log.FormattingEnabled = true;
            this.Log.HorizontalScrollbar = true;
            this.Log.Location = new System.Drawing.Point(22, 239);
            this.Log.Name = "Log";
            this.Log.Size = new System.Drawing.Size(294, 95);
            this.Log.TabIndex = 5;
            // 
            // RaysClientForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ClientSize = new System.Drawing.Size(335, 341);
            this.Controls.Add(this.Log);
            this.Controls.Add(groupBox1);
            this.Controls.Add(groupBox2);
            this.Controls.Add(groupBox3);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "RaysClientForm";
            this.Text = "Rays Client";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.onClose);
            this.Load += new System.EventHandler(this.RaysClientForm_Load);
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            groupBox2.ResumeLayout(false);
            groupBox2.PerformLayout();
            groupBox3.ResumeLayout(false);
            groupBox3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.TextBox port;
        private System.Windows.Forms.TextBox host;
        private System.Windows.Forms.Button Connect;
        private System.Windows.Forms.Button Open;
        private System.Windows.Forms.ListBox Log;
        private System.Windows.Forms.Button Quit;
        private System.Windows.Forms.ListBox Assets;
        private System.Windows.Forms.Button Render;
        private System.Windows.Forms.Button Configuration;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button SaveAs;
        private System.Windows.Forms.Button Resume;
        private System.Windows.Forms.Label Scene;
    }
}

