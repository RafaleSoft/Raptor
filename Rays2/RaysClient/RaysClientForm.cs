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
        }

        private void RaysClientForm_Load(object sender, EventArgs e)
        {

        }

        private void OnClick(object sender, EventArgs e)
        {
            // TODO: check active connections.
            DialogResult res = MessageBox.Show("Are you sure you want to quit ?","Information",MessageBoxButtons.YesNo);
            if (DialogResult.No != res)
                Close();
        }

        private void onOpen(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "(Description de scène)|*.xml";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                Log.Items.Add("Opening file: " + open.FileName);
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
    }
}
