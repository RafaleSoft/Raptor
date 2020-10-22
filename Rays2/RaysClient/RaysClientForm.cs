﻿/***************************************************************************/
/*                                                                         */
/*  RaysClientForm.cs                                                      */
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
using System.Diagnostics;
using System.Windows.Forms;
using System.IO;
using System.Collections.Specialized;


namespace RaysClient
{
    public partial class RaysClientForm : Form
    {
        public RaysClientForm()
        {
            InitializeComponent();

            Properties.Settings settings = Properties.Settings.Default;
            host.Text = settings.host;
            port.Text = settings.port.ToString();
            Render.Enabled = false;
            scene_filepath = "";

            network.SetLog(ref Log);
        }

        private void RaysClientForm_Load(object sender, EventArgs e)
        {

        }

        private void onClose(object sender, FormClosingEventArgs e)
        {
            if (network.IsConnected())
            {
                network.CloseSession();
                network.Disconnect();
            }
        }

        private void OnQuit(object sender, EventArgs e)
        {
            if (network.IsConnected())
            {
                DialogResult res = MessageBox.Show("A connection to the render server is still active. \nClosing will lose rendered data. \nAre you sure you want to quit ?", "Information", MessageBoxButtons.YesNo);
                if (DialogResult.No != res)
                {
                    network.CloseSession();
                    network.Disconnect();
                    Close();
                }
            }
            else
            {
                DialogResult res = MessageBox.Show("Are you sure you want to quit ?", "Information", MessageBoxButtons.YesNo);
                if (DialogResult.No != res)
                    Close();
            }
        }

        private void onOpen(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Description de scène (*.xml)|*.xml|Images (*.tga;*.jpg;*.png;*.bmp;*.tif;*.exr)|*.tga;*.jpg;*.png;*.bmp;*.tif;*.exr";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                Log.Items.Add("Opening file: " + open.FileName);
                string ext = Path.GetExtension(open.FileName);
                if ( ext == ".xml")
                {
                    scene_filepath = open.FileName;
                    Scene.Text = "Scene: " + Path.GetFileName(open.FileName);

                    Log.Items.Add("Loading scene: " + Path.GetFileName(open.FileName));
                }
                else
                {
                    if (supported_exts.Contains(ext))
                    {
                        Assets.Items.Add(Path.GetFileName(open.FileName));
                        assets.Add(open.FileName);
                        Log.Items.Add("Loading asset: " + Path.GetFileName(open.FileName));
                    }
                }
            }
            
            open.Dispose();

            this.Opacity = 1.0;
        }

        private void onPortChanged(object sender, EventArgs e)
        {
            short p = 0;
            bool b = short.TryParse(port.Text, out p);

            if (b && (p > 1024))
            {
                port.BackColor = System.Drawing.Color.LightGreen;
                Connect.Enabled = true;
            }
            else
            {
                port.BackColor = System.Drawing.Color.LightSalmon;
                Connect.Enabled = false;
            }
        }

        private void onIPChanged(object sender, EventArgs e)
        {
            System.Net.IPAddress ip;
            bool b = System.Net.IPAddress.TryParse(host.Text, out ip);

            if ((host.Text.Length > 0) && b)
            {
                host.BackColor = System.Drawing.Color.LightGreen;
                Connect.Enabled = true;
            }
            else
            {
                host.BackColor = System.Drawing.Color.LightSalmon;
                Connect.Enabled = false;
            }
        }

        private void onSceneChanged(object sender, EventArgs e)
        {
            if (Assets.Items.Count > 0)
            {
                DialogResult res = MessageBox.Show("Reset assets ?", "Information", MessageBoxButtons.YesNo);
                if (DialogResult.Yes == res)
                    Assets.Items.Clear();
            }

            if (network.IsConnected() && (scene_filepath.Length > 0))
                Render.Enabled = true;
            else
                Render.Enabled = false;
        }

        private void onConnect(object sender, EventArgs e)
        {
            Render.Enabled = false;

            if (network.IsConnected())
            {
                network.CloseSession();
                if (!network.Disconnect())
                    MessageBox.Show("Unable to disconnect from the render server", "Error", MessageBoxButtons.OK);
                else
                    Log.Items.Add("Disconnect from the render server");
                Connect.Text = "Connect";
            }
            else
            {
                short p = 0;
                bool b = short.TryParse(port.Text, out p);

                if (b && network.Connect(host.Text, p))
                {
                    if (network.IsConnected())
                    {
                        Log.Items.Add("Connected to server: " + host.Text + " on port " + port.Text);
                        Connect.Text = "Disconnect";

                        if (scene_filepath.Length > 0)
                            Render.Enabled = true;

                        if (!network.OpenSession())
                        {
                            Log.Items.Add("Echec d'ouverture de session Rays Server");
                            MessageBox.Show("Unable to open render server session", "Error", MessageBoxButtons.OK);
                        }
                    }
                    else
                        MessageBox.Show("Connection to Rays render server failed", "Error", MessageBoxButtons.OK);
                }
                else
                    MessageBox.Show("Unable to connect to the render server", "Error", MessageBoxButtons.OK);
            }
        }

        private void onRender(object sender, EventArgs e)
        {
            // Part 1: use ProcessStartInfo class.
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = Environment.ExpandEnvironmentVariables("%RAPTOR_ROOT%\\Redist\\Bin\\RaptorDataPackager.exe");
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;

            // Part 2: set arguments.
            startInfo.Arguments = "-c RaysData.pck " + scene_filepath;
            StringEnumerator it = assets.GetEnumerator();
            while (it.MoveNext())
                startInfo.Arguments += " " + it.Current;

            try
            {
                // Part 3: start with the info we specified.
                // ... Call WaitForExit.
                Process exeProcess = Process.Start(startInfo);
                exeProcess.WaitForExit();
            }
            catch
            {
                // Log error.
                Log.Items.Add("Failed to run RaptorDataPackager, rendering aborted !");
            }

            if (File.Exists("RaysData.pck"))
            {
                if (!network.SendJobData("RaysData.pck"))
                    Log.Items.Add("Echec d'envoi du package RaysData.pck à Rays Server");
                else
                    Log.Items.Add("RaysData.pck envoyé à Rays Server");

                Log.Items.Add("Rays Server starting rendering ...");
            }
            else
            {
                Log.Items.Add("RaysData.pck corrupted or not found, rendering aborted !");
            }
        }


        /**
         * Client Main data
         */
        private RaysClientNetwork network = new RaysClientNetwork();
        private String scene_filepath = new String("".ToCharArray());
        private StringCollection assets = new StringCollection();
        StringCollection supported_exts = new StringCollection { ".tga", ".jpg", ".png", ".bmp", ".exr", ".tif" };
    }
}
