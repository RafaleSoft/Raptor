using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using static ShaderCompiler.NativeWrapper;
using System.Runtime.InteropServices;

namespace ShaderCompiler
{
    public partial class ShaderCompiler : Form
    {
        public ShaderCompiler()
        {
            InitializeComponent();

            display = IntPtr.Zero;
            renderer = IntPtr.Zero;

            initialiseGL();
            renderTimer.Enabled = true;
        }

        private void onQuit(object sender, EventArgs e)
        {
            closeGL();

            Close();
        }

        private void onVertex(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Vertex Shader|*.vs;*.vert";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                string ext = Path.GetExtension(open.FileName).ToLower();
                if ((ext == ".vs") ||(ext == ".vert"))
                {
                    vertex_shader_file = open.FileName;
                    VShaderName.Text = Path.GetFileName(open.FileName);
                }
                else
                {
                    MessageBox.Show("Invalid vertex shader source file.", "Erreur", MessageBoxButtons.OK);
                }
            }

            open.Dispose();

            this.Opacity = 1.0;
        }

        private void onGeometry(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Geometry Shader|*.gs;*.geom";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                string ext = Path.GetExtension(open.FileName).ToLower();
                if ((ext == ".gs") || (ext == ".geom"))
                {
                    geometry_shader_file = open.FileName;
                    GShaderName.Text = Path.GetFileName(open.FileName);
                }
                else
                {
                    MessageBox.Show("Invalid geometry shader source file.", "Erreur", MessageBoxButtons.OK);
                }
            }

            open.Dispose();

            this.Opacity = 1.0;
        }

        private void onTesselate(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Tesselate Shader|*.ts;*.tess";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                string ext = Path.GetExtension(open.FileName).ToLower();
                if ((ext == ".ts") || (ext == ".tess"))
                {
                    tesselate_shader_file = open.FileName;
                    TShaderName.Text = Path.GetFileName(open.FileName);
                }
                else
                {
                    MessageBox.Show("Invalid tesselation shader source file.", "Erreur", MessageBoxButtons.OK);
                }
            }

            open.Dispose();

            this.Opacity = 1.0;
        }

        private void onFragment(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Fragment Shader|*.fs;*.frag";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                string ext = Path.GetExtension(open.FileName).ToLower();
                if ((ext == ".fs") || (ext == ".frag"))
                {
                    fragment_shader_file = open.FileName;
                    FShaderName.Text = Path.GetFileName(open.FileName);
                }
                else
                {
                    MessageBox.Show("Invalid fragment shader source file.", "Erreur", MessageBoxButtons.OK);
                }
            }

            open.Dispose();

            this.Opacity = 1.0;
        }

        private void onCompile(object sender, EventArgs e)
        {

        }

        private bool initialiseGL()
        {
            RAPTOR_CONFIG cfg = new RAPTOR_CONFIG("ShaderCompiler.log")
            {
                Relocation = 1,
                Vertices = 128 * 1024,
                Polygons = 8 * 1024
            };

            bool res = glInitRaptor(ref cfg);
            if (!res)
                MessageBox.Show("Unable to initialize Raptor", "Error", MessageBoxButtons.OK);
            Marshal.FreeHGlobal(cfg.logFile);

            RAPTOR_DISPLAY_CONFIG glcs = new RAPTOR_DISPLAY_CONFIG
            {
                x = 0,
                y = 0,
                width = Canvas.Width, //640,
                height = Canvas.Height, //480,
                display_mode = 0x02 + 0x20, // CGL_RGBA | CGL_DEPTH;
                acceleration = GL_ACCELERATION.HARDWARE,
                antialias = GL_ANTIALIAS.ANTIALIAS_NONE,
                renderer = GL_DISPLAY_RENDERER.NATIVE_GL,
                swap_buffer = GL_SWAPBUFFER.SWAP_EXCHANGE,
                bind_to_texture = 0,
                double_buffer = 1,
                depth_buffer = 1,
                stencil_buffer = 0,
                accumulator_buffer = 0,
                fps = 255,
                sync_to_monitor = 0,
                caption = Marshal.StringToHGlobalAnsi("ShaderCompiler")
            };

            res = res && glCreateDisplay(ref glcs, out display);
            if (!res)
                MessageBox.Show("Unable to create Raptor Display", "Error", MessageBoxButtons.OK);
            Marshal.FreeHGlobal(glcs.caption);

            canvas = Canvas.CreateGraphics();
            hDC = canvas.GetHdc();

            res = res && glBindDisplay(display, hDC);
            if (!res)
                MessageBox.Show("Unable to bind Raptor Display to canvas", "Error", MessageBoxButtons.OK);

            res = res && glCreateRaptorRenderer(display, out renderer);
            if (!res)
                MessageBox.Show("Unable to create Raptor Renderer", "Error", MessageBoxButtons.OK);

            diagGL();

            res = res && glUnBindDisplay(display);
            if (!res)
                MessageBox.Show("Unable to unbind Raptor Display from canvas", "Error", MessageBoxButtons.OK);

            return res;
        }

        private bool closeGL()
        {
            if (display.Equals(IntPtr.Zero))
                return true;

            bool res = glDestroyDisplay(display, hDC);
            if (!res)
                MessageBox.Show("Unable to destroy Raptor Display", "Error", MessageBoxButtons.OK);

            display = IntPtr.Zero;
            canvas.ReleaseHdc(hDC);
            canvas = null;

            res = res && glQuitRaptor();
            if (!res)
                MessageBox.Show("Unable to quit Raptor", "Error", MessageBoxButtons.OK);

            return res;
        }

        private void onPaint(object sender, PaintEventArgs e)
        {
            if (sender == Canvas)
            {
                // Cannot use the gc provided by paint event: must use the DC from graphics initially created
                //Graphics g = e.Graphics;
                //IntPtr hDC = g.GetHdc();

                if (glBindDisplay(display, hDC))
                {
                    glRender();
                    glUnBindDisplay(display);
                }

                //g.ReleaseHdc(hDC);
            }
        }

        private void onTick(object sender, EventArgs e)
        {
            //Canvas.Invalidate();
            if (glBindDisplay(display, hDC))
            {
                glRender();
                glUnBindDisplay(display);
            }
        }

        private void onGLDiag(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            GLDiag.ShowDialog();

            this.Opacity = 1.0;
        }

        private void onVulkanDiag(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            VKDiag.ShowDialog();

            this.Opacity = 1.0;
        }

        private void onDiffuse(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Image File|*.jpg;*.jpeg;*.tga;*.png;*.exr;*.tiff;*.tif";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                string ext = Path.GetExtension(open.FileName).ToLower();
                if ((ext == ".jpg") || (ext == ".jpeg") || (ext == ".tga") || (ext == ".png") || (ext == ".exr") || (ext == ".tiff") || (ext == ".tif"))
                {
                    diffuse_map_file = open.FileName;
                    DMapName.Text = Path.GetFileName(open.FileName);
                }
                else
                {
                    MessageBox.Show("Invalid Image file format for diffuse texture map: " + ext + " is not supported.", "Erreur", MessageBoxButtons.OK);
                }
            }

            open.Dispose();

            IntPtr d = Marshal.StringToHGlobalAnsi(diffuse_map_file);
            IntPtr n = Marshal.StringToHGlobalAnsi(normal_map_file);
            if (glBindDisplay(display, hDC))
            {
                glSetMaps(d, n);
                glUnBindDisplay(display);
            }

            this.Opacity = 1.0;
        }

        private void onNormal(object sender, EventArgs e)
        {
            this.Opacity = 0.5;

            FileDialog open = new OpenFileDialog();
            open.Filter = "Image File|*.jpg;*.jpeg;*.tga;*.png;*.exr;*.tiff;*.tif";
            DialogResult res = open.ShowDialog();

            if (DialogResult.OK == res)
            {
                string ext = Path.GetExtension(open.FileName).ToLower();
                if ((ext == ".jpg") || (ext == ".jpeg") || (ext == ".tga") || (ext == ".png") || (ext == ".exr") || (ext == ".tiff") || (ext == ".tif"))
                {
                    normal_map_file = open.FileName;
                    NMapName.Text = Path.GetFileName(open.FileName);
                }
                else
                {
                    MessageBox.Show("Invalid Image file format for nomal texture map: " + ext + " is not supported.", "Erreur", MessageBoxButtons.OK);
                }
            }

            IntPtr d = Marshal.StringToHGlobalAnsi(diffuse_map_file);
            IntPtr n = Marshal.StringToHGlobalAnsi(normal_map_file);
            if (glBindDisplay(display, hDC))
            {
                glSetMaps(d, n);
                glUnBindDisplay(display);
            }

            open.Dispose();

            this.Opacity = 1.0;
        }

        private void diagGL()
        {
            bool res = glDiag();
            if (!res)
                MessageBox.Show("Unable to query OpenGL Diagnostics", "Error", MessageBoxButtons.OK);

            string diag_file = System.IO.Path.GetTempPath() + "\\gldiag.txt";
            StreamReader ReaderObject = new StreamReader(diag_file);

            GLDiag.Vendor.Text = ReaderObject.ReadLine();
            GLDiag.Renderer.Text = ReaderObject.ReadLine();
            GLDiag.Version.Text = ReaderObject.ReadLine();
            GLDiag.GLSL.Text = ReaderObject.ReadLine();

            GLDiag.GLLimits.SuspendLayout();
            Control.ControlCollection collection = GLDiag.GLLimits.Controls;

            int nb_limits = Int32.Parse(ReaderObject.ReadLine());
            for (int i = 0; i < nb_limits; i++)
            {
                string param = ReaderObject.ReadLine();
                string[] values = param.Split(' ');

                Label lbl = new Label();
                lbl.AutoSize = true;
                lbl.Location = new System.Drawing.Point(16, 5 + i * 16);
                lbl.Name = "label" + (2*i).ToString();
                lbl.Size = new System.Drawing.Size(35, 13);
                lbl.TabIndex = 0;
                lbl.Text = values[0];
                collection.Add(lbl);

                lbl = new Label();
                lbl.AutoSize = true;
                lbl.Location = new System.Drawing.Point(255, 5 + i * 16);
                lbl.Name = "label" + (2*i+1).ToString();
                lbl.Size = new System.Drawing.Size(13, 13);
                lbl.TabIndex = 7;
                lbl.Text = values[1];
                collection.Add(lbl);
            }

            GLDiag.GLLimits.ResumeLayout(false);
            GLDiag.GLLimits.PerformLayout();

            GLDiag.GLExtensions.SuspendLayout();
            while (!ReaderObject.EndOfStream)
                GLDiag.GLExtensions.Items.Add(ReaderObject.ReadLine());
            GLDiag.GLExtensions.ResumeLayout(true);

            ReaderObject.Close();
        }

        private OpenGLDiag GLDiag = new OpenGLDiag();
        private VulkanDiag VKDiag = new VulkanDiag();

        private Graphics canvas = null;
        private IntPtr display;
        private IntPtr renderer;
        private IntPtr hDC;

        private String vertex_shader_file = "";
        private String geometry_shader_file = "";
        private String tesselate_shader_file = "";
        private String fragment_shader_file = "";
        private String diffuse_map_file = "";
        private String normal_map_file = "";
    }
}
