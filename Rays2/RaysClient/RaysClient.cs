/***************************************************************************/
/*                                                                         */
/*  RaysClient.cs                                                          */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

namespace RaysClient
{
    public class RaysClient
    {
        public struct CFGSTRUCT
        {
            public Int32   width;
            public Int32   height;
            public Double  variance;
            public Int32   reflection_depth;
            public Int32   refraction_depth;
            public Double  crease;
            public Double  focale;
            public Double  object_plane;
            public Double  aperture;
        }

        public RaysClient()
        {
            config.width = 320;
            config.height = 240;
            config.variance = 0.01;
            config.reflection_depth = 5;
            config.refraction_depth = 5;
            config.crease = 0;
            config.focale = 50.0;
            config.object_plane = 10.0;
            config.aperture = 2.8;
        }

        public CFGSTRUCT GetConfig()
        {
            return config;
        }

        public void SetConfig(ref CFGSTRUCT cfg)
        {
            config = cfg;
        }

        public bool Render(string assets)
        {
            // Part 1: use ProcessStartInfo class.
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = Environment.ExpandEnvironmentVariables("%RAPTOR_ROOT%\\Redist\\Bin\\RaptorDataPackager.exe");
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;

            // Part 2: set arguments.
            startInfo.Arguments = "-c RaysData.pck " + assets;

            try
            {
                // Part 3: start with the info we specified.
                // ... Call WaitForExit.
                Process exeProcess = Process.Start(startInfo);
                exeProcess.WaitForExit();
            }
            catch
            {
                log.Error("Failed to run RaptorDataPackager, rendering aborted !");
                return false;
            }

            if (File.Exists("RaysData.pck"))
            {
                FileStream fs = File.Open("RaysData.pck", FileMode.Open, FileAccess.Read);

                BinaryReader SourceStream = new BinaryReader(fs);
                byte[] data = SourceStream.ReadBytes((int)fs.Length);

                fs.Close();
                SourceStream.Dispose();

                if (!network.SendMessage(RaysClientNetwork.JOB_DATA, 0, 0, 0, 0, 0, ref data))
                {
                    log.Error("Echec d'envoi du package RaysData.pck à Rays Server");
                    return false;
                }
                else
                    log.Info("RaysData.pck envoyé à Rays Server");
            }
            else
            {
                log.Error("RaysData.pck corrupted or not found !");
                return false;
            }

            // Part 4: start job with current client rendering configuration.
            int len = Marshal.SizeOf(config);
            byte[] cfg = new byte[len];

            System.IntPtr ptr = Marshal.AllocHGlobal(len);
            Marshal.StructureToPtr(config, ptr, true);
            Marshal.Copy(ptr, cfg, 0, len);
            Marshal.FreeHGlobal(ptr);
                        
            if (!network.SendMessage(RaysClientNetwork.JOB_START, 0, 0, 0, 0, 0, ref cfg))
            {
                log.Error("Echec d'envoi du package RaysData.pck à Rays Server");
                return false;
            }
            else
            {
                RaysClientNetwork.MSGSTRUCT msg = new RaysClientNetwork.MSGSTRUCT();
                byte[] messageData = null;
                if (network.ReceiveMessage(ref msg, ref messageData))
                {
                    if (RaysClientNetwork.JOB_ID == msg.msg_id)
                    {
                        job_id = (msg.msg_data1 << 32) + msg.msg_data0;
                        if (job_id > 0)
                            log.Info("Lancement du rendu par Rays Server du job: " + job_id.ToString());
                        else
                            log.Info("Avortement du rendu par Rays Server: par de workunit libre");
                    }
                    else
                    {
                        log.Error("Erreur de protocole de job Rays Server");
                        return false;
                    }
                }
                else
                {
                    log.Error("Echec de lancement du rendu par Rays Server");
                    return false;
                }
            }

            return true;
        }

        public bool Connect(string host, short port)
        {
            if (network.Connect(host, port))
            {
                if (network.IsConnected())
                {
                    if (!OpenSession())
                        log.Error("Echec d'ouverture de session Rays Server");
                    else
                        return true;
                }
            }

            return false;
        }

        public bool Disconnect()
        {
            if (!CloseSession())
                log.Error("Echec de fermeture de session Rays Server. Connection perdue ?");

            return network.Disconnect();
        }

        public bool IsConnected()
        {
            return network.IsConnected();
        }

        public bool OpenSession()
        {
            byte[] nodata = new byte[0];
            if (network.SendMessage(RaysClientNetwork.SES_OPEN, 0, 0, 0, 0, 0, ref nodata))
            {
                RaysClientNetwork.MSGSTRUCT msg = new RaysClientNetwork.MSGSTRUCT();
                byte[] messageData = null;
                if (network.ReceiveMessage(ref msg, ref messageData))
                {
                    if (RaysClientNetwork.SES_ID == msg.msg_id)
                    {
                        session_id = (msg.msg_data1 << 32) + msg.msg_data0;
                        log.Info("Ouverture de session Rays Server:" + session_id.ToString());
                        return true;
                    }
                    else
                    {
                        log.Error("Erreur de protocole de session Rays Server");
                        return false;
                    }
                }
                else
                {
                    log.Error("Echec d'allocation d'identifiant de session Rays Server");
                    return false;
                }
            }
            else
                return false;
        }

        public bool CloseSession()
        {
            log.Info("Fermeture de session Rays Server:" + session_id.ToString());
            session_id = 0;

            byte[] nodata = new byte[0];
            return network.SendMessage(RaysClientNetwork.SES_CLOSE, 0, 0, 0, 0, 0, ref nodata);
        }

        public bool Close()
        {
            bool res = true;

            if (network.IsConnected())
            {
                res = res && CloseSession();
                res = res && network.Disconnect();
            }

            return res;
        }

        public void SetLog(ref RaysLogger l)
        {
            log = l;
            network.SetLog(ref l);
        }

        private RaysClientNetwork network = new RaysClientNetwork();
        private RaysLogger log = null;
        private ulong session_id = 0;
        private uint job_id = 0;
        private CFGSTRUCT config = new CFGSTRUCT();
    }
}
