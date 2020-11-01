using System;
using System.Windows.Forms;

namespace RaysClient
{
    public partial class RaysConfig : Form
    {
        public const Int32 MAX_WIDTH = 65536;
        public const Int32 MAX_HEIGHT = 65536;
        public const Int32 MAX_REFLECTION = 32;
        public const Int32 MAX_REFRACTION = 32;
        public const Double MAX_VARIANCE = 1.0;
        public const Double MAX_CREASE = 0.0;
        public const Double MAX_APERTURE = 5.0;
        public const Double MIN_APERTURE = 1.0;
        public const Double MAX_FOCALE = 500.0;
        public const Double MIN_FOCALE = 10.0;
        public const Double MAX_PLANE = 1000.0;
        public const Double MIN_PLANE = 1.0;

        public RaysConfig()
        {
            InitializeComponent();
        }

        public void SetClient(ref RaysClient cfg)
        {
            client = cfg;
        }

        private void onWidth(object sender, EventArgs e)
        {
            CheckField(ref RenderingWidth, 0, MAX_WIDTH);
        }

        private void onHeight(object sender, EventArgs e)
        {
            CheckField(ref RenderingHeight, 0, MAX_HEIGHT);
        }

        private void onApply(object sender, EventArgs e)
        {
            RaysClient.CFGSTRUCT cfg = new RaysClient.CFGSTRUCT();

            bool b = true;
            b = b && Int32.TryParse(RenderingWidth.Text, out cfg.width);
            b = b && Int32.TryParse(RenderingHeight.Text, out cfg.height);
            b = b && Int32.TryParse(Reflection.Text, out cfg.reflection_depth);
            b = b && Int32.TryParse(Refraction.Text, out cfg.refraction_depth);
            b = b && Double.TryParse(Variance.Text, out cfg.variance);
            b = b && Double.TryParse(Crease.Text, out cfg.crease);
            b = b && Double.TryParse(Aperture.Text, out cfg.aperture);
            b = b && Double.TryParse(Focale.Text, out cfg.focale);
            b = b && Double.TryParse(ObjectPlane.Text, out cfg.object_plane);

            client.SetConfig(ref cfg);
        }

        private void onCancel(object sender, EventArgs e)
        {

        }

        private void onPlane(object sender, EventArgs e)
        {
            CheckField(ref ObjectPlane, MIN_PLANE, MAX_PLANE);
        }

        private void onReflection(object sender, EventArgs e)
        {
            CheckField(ref Reflection, 0, MAX_REFLECTION);
        }

        private void onRefraction(object sender, EventArgs e)
        {
            CheckField(ref Refraction, 0, MAX_REFRACTION);
        }

        private void onVariance(object sender, EventArgs e)
        {
            CheckField(ref Variance, 0, MAX_VARIANCE);
        }

        private void onCrease(object sender, EventArgs e)
        {
            CheckField(ref Crease, 0, MAX_CREASE);
        }

        private void onFocale(object sender, EventArgs e)
        {
            CheckField(ref Focale, MIN_FOCALE, MAX_FOCALE);
        }

        private void onAperture(object sender, EventArgs e)
        {
           CheckField(ref Aperture, MIN_APERTURE, MAX_APERTURE);
        }

        private void CheckField(ref TextBox box, Int32 min, Int32 max)
        {
            Int32 m = 0;
            bool b = System.Int32.TryParse(box.Text, out m);
            if (b && (m >= min) && (m <= max))
            {
                box.BackColor = System.Drawing.Color.LightGreen;
                Apply.Enabled = true;
            }
            else
            {
                box.BackColor = System.Drawing.Color.LightSalmon;
                Apply.Enabled = false;
            }
        }

        private void CheckField(ref TextBox box, Double min, Double max)
        {
            Double m = 0;
            bool b = System.Double.TryParse(box.Text, out m);
            if (b && (m >= min) && (m <= max))
            {
                box.BackColor = System.Drawing.Color.LightGreen;
                Apply.Enabled = true;
            }
            else
            {
                box.BackColor = System.Drawing.Color.LightSalmon;
                Apply.Enabled = false;
            }
        }

        private RaysClient client = null;

        private void onLoad(object sender, EventArgs e)
        {
            if (null == client)
                return;

            RaysClient.CFGSTRUCT cfg = client.GetConfig();
            
            RenderingWidth.Text = cfg.width.ToString();
            RenderingHeight.Text = cfg.height.ToString();
            Reflection.Text = cfg.reflection_depth.ToString();
            Refraction.Text = cfg.refraction_depth.ToString();
            Variance.Text = cfg.variance.ToString();
            Crease.Text = cfg.crease.ToString();
            Aperture.Text = cfg.aperture.ToString();
            Focale.Text = cfg.focale.ToString();
            ObjectPlane.Text = cfg.object_plane.ToString();
        }
    }
}
