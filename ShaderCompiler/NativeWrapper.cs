using System;
using System.Runtime.InteropServices;


namespace ShaderCompiler
{
    class NativeWrapper
    {
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct RAPTOR_CONFIG
        {
            public IntPtr   logFile;
            public byte     autoDestroy;
            public byte     Relocation;
            public byte     Compute;
            public byte     Vulkan;
            public ulong    Polygons;
            public ulong    Vertices;
            public ulong    Texels;
            public ulong    Uniforms;
            public ulong    garbageSize;
            public float    sizeFactor;

            public RAPTOR_CONFIG(string log)
            {
                logFile = Marshal.StringToHGlobalAnsi(log);
                autoDestroy = 1;
                Relocation = 0;
                Compute = 0;
                Vulkan = 0;
                Polygons = 1024;
                Vertices = 16 * 1024;
                Texels = 1024 * 1024;
                Uniforms = 128 * 1024;
                garbageSize = 1024 * 1024;
                sizeFactor = 1.0f;
            }
        };

        public enum GL_ACCELERATION
        {
            SOFTWARE,
            GENERIC,
            HARDWARE
        }

        public enum GL_ANTIALIAS
        {
            ANTIALIAS_NONE,
            ANTIALIAS_2X,
            ANTIALIAS_4X,
            ANTIALIAS_5X,
            ANTIALIAS_6X,
            ANTIALIAS_8X,
            ANTIALIAS_16X,
            ANTIALIAS_32X
        }

        public enum GL_SWAPBUFFER
        {
            SWAP_UNDEFINED,
            SWAP_COPY,
            SWAP_EXCHANGE
        }

        public enum GL_DISPLAY_RENDERER
        {
            NATIVE_GL,      // ScreenDisplay
            RENDER_BUFFER_FILTER_CHAIN,
            PIXEL_BUFFER_FILTER_CHAIN,
            RENDER_BUFFER,
            PIXEL_BUFFER,
            VULKAN
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct RAPTOR_DISPLAY_CONFIG
        {
            public int  x;
            public int  y;
            public int  width;
            public int  height;
            public byte draw_logo;
            public byte status_bar;
            public uint display_mode;
            public GL_ACCELERATION  acceleration;
            public GL_ANTIALIAS     antialias;
            public GL_SWAPBUFFER    swap_buffer;
            public GL_DISPLAY_RENDERER renderer;
            public byte bind_to_texture;
            public byte overlay;
            public byte double_buffer;
            public byte depth_buffer;
            public byte stencil_buffer;
            public byte accumulator_buffer;
            public byte fps;
            public byte sync_to_monitor;
            public IntPtr caption;
        };


        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glInitRaptor(ref RAPTOR_CONFIG config);

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glQuitRaptor();

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glCreateDisplay(ref RAPTOR_DISPLAY_CONFIG config, out IntPtr display);

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glDestroyDisplay(IntPtr display, IntPtr dc);

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glBindDisplay(IntPtr display, IntPtr dc);

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glUnBindDisplay(IntPtr display);

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glRender();

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glCreateRaptorRenderer(IntPtr display, out IntPtr renderer);

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glDiag();

        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glSetMaps(IntPtr diffuse, IntPtr normal);


        [DllImport("ShaderCompilerWrapper.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glSetShaders(IntPtr vertex, IntPtr geometry, IntPtr fragment, out IntPtr log);
    }
}
