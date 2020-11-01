/***************************************************************************/
/*                                                                         */
/*  RaysLogger.cs                                                          */
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



using System.IO;
using System.Windows.Forms;


namespace RaysClient
{
    public class RaysLogger
    {
        public enum LEVEL
        {
            DEBUG,
            INFO,
            WARNING,
            ERROR
        }

        public void SetLogName(string name)
        {
            if (null != writer)
                writer.Close();

            writer = File.CreateText(name);
        }

        public void SetDisplay(ref ListBox lb)
        {
            log_display = lb;
        }

        public void SetLevel(LEVEL l)
        {
            level = l;
        }


        public void Debug(string message)
        {
            if (level <= LEVEL.DEBUG)
            {
                string log = "[DEBUG] " + System.DateTime.Now.ToString() + " - " + message;
                Write(ref log);
            }
        }

        public void Info(string message)
        {
            if (level <= LEVEL.INFO)
            {
                string log = "[INFO] " + System.DateTime.Now.ToString() + " - " + message;
                Write(ref log);
            }
        }

        public void Warning(string message)
        {
            if (level <= LEVEL.WARNING)
            {
                string log = "[WARNING] " + System.DateTime.Now.ToString() + " - " + message;
                Write(ref log);
            }
        }

        public void Error(string message)
        {
            if (level <= LEVEL.ERROR)
            {
                string log = "[ERROR] " + System.DateTime.Now.ToString() + " - " + message;
                Write(ref log);
            }
        }

        ~RaysLogger()
        {
            if (null != writer)
            {
                writer.Close();
                writer.Dispose();
            }
        }

        private void Write(ref string message)
        {
            if (null != writer)
            {
                writer.WriteLineAsync(message);
            }

            if (null != log_display)
            {
                log_display.Items.Add(message);
            }
        }

        private StreamWriter writer = null;
        private ListBox log_display = null;
        private LEVEL level = LEVEL.WARNING;
    }
}
