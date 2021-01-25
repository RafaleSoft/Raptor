using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ShaderCompiler
{
    public partial class VulkanDiag : Form
    {
        public VulkanDiag()
        {
            InitializeComponent();
        }

        private void onClose(object sender, EventArgs e)
        {
            Close();
        }
    }
}
