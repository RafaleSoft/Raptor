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
        }

        private void RaysClientForm_Load(object sender, EventArgs e)
        {

        }

        private void OnClick(object sender, EventArgs e)
        {
            Close();
        }
    }
}
