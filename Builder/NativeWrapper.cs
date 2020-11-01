/***************************************************************************/
/*                                                                         */
/*  NativeWrapper.cs                                                       */
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
        public bool             active;         // beware : bool size is 4 here, C size is 1. works because of alignment.
        public string           extensionName;
        public string[]         dependencies;
	}

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct BUILD_SUPPLEMENT
    {
        public bool DEBUG;
        public bool REDIST;
        public bool COMPUTE;
        public bool PHYSX;
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

                        string extName = Marshal.PtrToStringAnsi(exts[i].extensionName);

                        extensions[i].kind = exts[i].kind;
                        extensions[i].active = exts[i].active;
                        extensions[i].extensionName = extName;
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

        public static bool setBuildSupplement(ref BUILD_SUPPLEMENT bld)
        {
            NATIVE_SUPPLEMENT native = new NATIVE_SUPPLEMENT();
            native.COMPUTE = (bld.COMPUTE ? (byte)1 : (byte)0);
            native.DEBUG = (bld.DEBUG ? (byte)1 : (byte)0);
            native.REDIST = (bld.REDIST ? (byte)1 : (byte)0);
            native.PHYSX = (bld.PHYSX ? (byte)1 : (byte)0);
            return setBuildSupplement(ref native);
        }

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool checkConsistency(bool force);

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool writeHeader(string filename);

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool activateExtension(string extension, bool activate);

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool activateAllOrNone(bool all);


        //!
        //! Private wrapper helpers
        //!

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct NATIVE_EXTENSION
        {
            public EXTENSION_KIND kind;
            public bool active;
            public IntPtr extensionName;
            public uint nb_dependencies;
            public IntPtr dependencies;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct NATIVE_SUPPLEMENT
        {
            public byte DEBUG;
            public byte REDIST;
            public byte COMPUTE;
            public byte PHYSX;
        }

        private static List<string> GetAllStrings(IntPtr ptr, int size)
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

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool getExtensions([In, Out] NativeWrapper.NATIVE_EXTENSION[] ext, ref uint s);

        [DllImport("BuilderNative.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool setBuildSupplement(ref NATIVE_SUPPLEMENT bld);
    }
}
