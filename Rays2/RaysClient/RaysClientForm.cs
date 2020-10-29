/***************************************************************************/
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

            logger.SetLevel(RaysLogger.LEVEL.INFO);
            logger.SetLogName(settings.logfile);
            logger.SetDisplay(ref Log);
            client.SetLog(ref logger);
        }

        private void RaysClientForm_Load(object sender, EventArgs e)
        {

        }

        private void onClose(object sender, FormClosingEventArgs e)
        {
            if (!client.Close())
                MessageBox.Show("Closing Rays Session of Rays Server disconnect failed", "Error", MessageBoxButtons.OK);
        }

        private void OnQuit(object sender, EventArgs e)
        {
            if (client.IsConnected())
            {
                DialogResult res = MessageBox.Show("A connection to the render server is still active. \nClosing will lose rendered data. \nAre you sure you want to quit ?", "Information", MessageBoxButtons.YesNo);
                if (DialogResult.No != res)
                {
                    client.Close();
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
                logger.Info("Opening file: " + open.FileName);
                string ext = Path.GetExtension(open.FileName);
                if ( ext == ".xml")
                {
                    scene_filepath = open.FileName;
                    Scene.Text = "Scene: " + Path.GetFileName(open.FileName);

                    logger.Info("Loading scene: " + Path.GetFileName(open.FileName));
                }
                else
                {
                    if (supported_exts.Contains(ext))
                    {
                        Assets.Items.Add(Path.GetFileName(open.FileName));
                        assets.Add(open.FileName);
                        logger.Info("Loading asset: " + Path.GetFileName(open.FileName));
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

            if (client.IsConnected() && (scene_filepath.Length > 0))
                Render.Enabled = true;
            else
                Render.Enabled = false;
        }

        private void onConnect(object sender, EventArgs e)
        {
            Render.Enabled = false;

            if (client.IsConnected())
            {
                if (!client.Disconnect())
                    MessageBox.Show("Unable to disconnect from the render server", "Error", MessageBoxButtons.OK);
                else
                    logger.Info("Disconnect from the render server");
                Connect.Text = "Connect";
            }
            else
            {
                short p = 0;
                bool b = short.TryParse(port.Text, out p);

                if (b && client.Connect(host.Text, p))
                {
                     logger.Info("Connected to server: " + host.Text + " on port " + port.Text);
                     Connect.Text = "Disconnect";
                     if (scene_filepath.Length > 0)
                        Render.Enabled = true;
                }
                else
                    MessageBox.Show("Unable to connect to the render server", "Error", MessageBoxButtons.OK);
            }
        }

        private void onRender(object sender, EventArgs e)
        {
            string render_assets = scene_filepath;
            StringEnumerator it = assets.GetEnumerator();
            while (it.MoveNext())
                render_assets += " " + it.Current;

            if (client.Render(render_assets))
                logger.Info("Rays Server starting rendering ...");
            else
                logger.Error("Errors found in rendering process, rendering aborted !");
        }

        private void onConfig(object sender, EventArgs e)
        {

        }

        private void onSaveAs(object sender, EventArgs e)
        {

        }

        private void onResume(object sender, EventArgs e)
        {

        }

        /**
         * Client Main data
         */
        private RaysClient client = new RaysClient();
        private RaysConfig config = new RaysConfig();
        private string scene_filepath = new string("".ToCharArray());
        private StringCollection assets = new StringCollection();
        private RaysLogger logger = new RaysLogger();
        StringCollection supported_exts = new StringCollection { ".tga", ".jpg", ".png", ".bmp", ".exr", ".tif" };
    }
}
