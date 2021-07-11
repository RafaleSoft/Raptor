using System;
using System.Windows.Forms;

namespace ShaderCompiler
{
    public partial class OpenGLDiag : Form
    {
        public OpenGLDiag()
        {
            InitializeComponent();
        }

        private void onClose(object sender, EventArgs e)
        {
            Close();
        }
    }
}
