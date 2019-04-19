using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace Builder
{
    public enum EXTENSION_KIND
	{
		CPU,
		COREGL,
		GLPROFILE,
		COREVK,
		ARB,
		VK,
		EXT,
		NV,
        APPLE,
        ATI,
        HP,
        IBM,
        SGIS,
        SUN,
		WGL,
		GLX,
		KHR,
        MESA,
		OES,
		_3DFX
	}

    public struct EXTENSION
	{
		public EXTENSION_KIND	kind;
        public bool             active;
        public string           extensionName;
        public string[]         dependencies;
	}

    class NativeWrapper
    {
        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool glvkInitBuilder(IntPtr dc);
        
        public static Builder.EXTENSION[] queryExtensions()
        {
            uint size = 0;
            bool res = NativeWrapper.getExtensions(null, ref size);
            if (res && size > 0)
            {
                NativeWrapper.NATIVE_EXTENSION[] exts = new NativeWrapper.NATIVE_EXTENSION[size];
                res = NativeWrapper.getExtensions(exts, ref size);

                if (res)
                {
                    EXTENSION[] extensions = new EXTENSION[size];
                    for (int i = 0; i < size; i++)
                    {
                        IntPtr ptr = exts[i].dependencies;
                        int nb = (int)exts[i].nb_dependencies;

                        List<string> ls = GetAllStrings(ptr, nb);

                        extensions[i].kind = exts[i].kind;
                        extensions[i].active = exts[i].active;
                        extensions[i].extensionName = exts[i].extensionName;
                        extensions[i].dependencies = ls.ToArray();
                    }

                    return extensions;
                }
                else
                    return null;
            }
            else
                return null;
        }

        public static List<string> GetAllStrings(IntPtr ptr, int size)
        {
            var list = new List<string>();
            for (int i = 0; i < size; i++)
            {
                var strPtr = (IntPtr)Marshal.PtrToStructure(ptr, typeof(IntPtr));
                list.Add(Marshal.PtrToStringAnsi(strPtr));
                ptr = new IntPtr(ptr.ToInt64() + IntPtr.Size);
            }
            return list;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct NATIVE_EXTENSION
        {
            public EXTENSION_KIND kind;
            public bool active;
            public string extensionName;
            public uint nb_dependencies;
            public IntPtr dependencies;
        };

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool getExtensions([In, Out] NativeWrapper.NATIVE_EXTENSION[] ext, ref uint s);
    }
}
