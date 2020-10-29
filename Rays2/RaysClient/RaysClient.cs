/***************************************************************************/
/*                                                                         */
/*  RaysClient.cs                                                          */
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
using System.Diagnostics;
using System.IO;


namespace RaysClient
{
    class RaysClient
    {
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

            byte[] nodata = new byte[6];
            if (!network.SendMessage(RaysClientNetwork.JOB_START, 0, 0, 0, 0, 0, ref nodata))
            {
                log.Error("Echec d'envoi du package RaysData.pck à Rays Server");
                return false;
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
            if (CloseSession())
                return network.Disconnect();
            else
                return false;
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
    }
}
