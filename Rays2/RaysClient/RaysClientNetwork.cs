using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace RaysClient
{
    class RaysClientNetwork
    {
        struct MSGSTRUCT
        {
            // Message header
            public uint msg_crc;
            public uint msg_size;       // size of following chunck of data

            // Message body
            public uint msg_id;         // semantic
            public uint msg_data0;      // sub semantic ID
            public uint msg_data1;
            public uint msg_data2;
            public uint msg_data3;
            public uint msg_data4;
        }

        // Session Messages semantic
        public const uint SES_OPEN = 0x01000000 + 0x01;
        public const uint SES_CLOSE = 0x01000000 + 0x02;
        public const uint SES_ID = 0x01000000 + 0x03;

        // Job Messages semantic
        public const uint JOB_DATA = 0x00000000 + 0x0f;


        public bool OpenSession()
        {
            byte[] nodata = new byte[0];
            if (SendMessage(SES_OPEN, 0, 0, 0, 0, 0, ref nodata))
            {
                MSGSTRUCT msg = new MSGSTRUCT();
                byte[] messageData = null;
                if (ReceiveMessage(ref msg, ref messageData))
                {
                    if (SES_ID == msg.msg_id)
                    {
                        session_id = (msg.msg_data1 << 32) + msg.msg_data0;
                        log.Items.Add("Ouverture de session Rays Server:" + session_id.ToString());
                        return true;
                    }
                    else
                    {
                        log.Items.Add("Erreur de protocole de session Rays Server");
                        return false;
                    }
                }
                else
                {
                    log.Items.Add("Echec d'allocation d'identifiant de session Rays Server");
                    return false;
                }
            }
            else
                return false;
        }

        public bool CloseSession()
        {
            byte[] nodata = new byte[0];
            return SendMessage(SES_CLOSE, 0, 0, 0, 0, 0, ref nodata);
        }

        public bool SendJobData(string package)
        {
            if (File.Exists(package))
            {
                FileStream fs = File.Open(package, FileMode.Open, FileAccess.Read);

                BinaryReader SourceStream = new BinaryReader(fs);
                byte[] data = SourceStream.ReadBytes((int)fs.Length);

                fs.Close();
                SourceStream.Dispose();

                return SendMessage(JOB_DATA, 0, 0, 0, 0, 0, ref data);
            }
            else
                return false;
        }

        public bool Connect(string address, short port)
        {
            if (server == null)
            {
                server = new Socket(AddressFamily.InterNetwork, // IP v4
                                    SocketType.Stream,
                                    ProtocolType.Tcp);
            }

            server.Blocking = true;
            IPAddress addr = IPAddress.Parse(address);
            if (addr.AddressFamily != AddressFamily.InterNetwork)
            {
                // TODO: log not supported address family (v6)
                log.Items.Add("Addressage IPv6 non supporté: " + address);
                return false;
            }

            bool connect = false;
            try
            {
                server.Connect(addr,port);
                connect = server.Connected;
            }
            catch(SocketException e)
            {
                log.Items.Add("Echec de connection à Rays Server: " + e.Message);
            }

            return connect;
        }

        public bool Disconnect()
        {
            if (null == server)
                return false;

            if (server.Connected)
            {
                server.Close();
                server = null;
            }

            return true;
        }

        public bool IsConnected()
        {
            if (null == server)
                return false;
            
            return server.Connected;
        }

        ~RaysClientNetwork()
        {
            if (Disconnect())
                server.Dispose();
        }

        public RaysClientNetwork()
        {
        }

        public void SetLog(ref ListBox lb)
        {
            log = lb;
        }

        private bool SendMessage(uint messageId, uint data0, uint data1, uint data2, uint data3, uint data4, ref byte[] messageData)
        {
            MSGSTRUCT msg = new MSGSTRUCT();
            msg.msg_crc = 0;        // not yet active
            msg.msg_size = (uint)Marshal.SizeOf(msg) - 2 * sizeof(uint); // crc & size are not part of the message, but only protocol.
            msg.msg_size += (uint)messageData.GetLength(0);
            msg.msg_data0 = data0;
            msg.msg_data1 = data1;
            msg.msg_data2 = data2;
            msg.msg_data3 = data3;
            msg.msg_data4 = data4;
            msg.msg_id = messageId;

            int len = Marshal.SizeOf(msg);
            int datalen = len + messageData.GetLength(0);
            byte[] sendBuffer = new byte[datalen];

            int result = 0;
            try
            {
                System.IntPtr ptr = Marshal.AllocHGlobal(len);
                Marshal.StructureToPtr(msg, ptr, true);
                Marshal.Copy(ptr, sendBuffer, 0, len);
                Marshal.FreeHGlobal(ptr);

                if (messageData.GetLength(0) > 0)
                    messageData.CopyTo(sendBuffer, len);

                while (result < datalen)
                    result += server.Send(sendBuffer, result, datalen - result, SocketFlags.None);
            }
            catch (SocketException e)
            {
                log.Items.Add("Erreur d'envoi de message à Rays Server: " + e.Message);
            }

            return (result == datalen);
        }

        private bool ReceiveMessage(ref MSGSTRUCT msg, ref byte[] messageData)
        {
            int len = Marshal.SizeOf(msg);

            byte[] receiveBuffer = new byte[len];
            int result = 0;
            try
            {
                while (result < len)
                    result += server.Receive(receiveBuffer, result, len - result, SocketFlags.None);

                System.IntPtr ptr = Marshal.AllocHGlobal(len);
                Marshal.Copy(receiveBuffer, 0, ptr, len);
                msg = Marshal.PtrToStructure<MSGSTRUCT>(ptr);
                Marshal.FreeHGlobal(ptr);

                int datalen = (int)(msg.msg_size + 2 * sizeof(uint) - len);
                if (datalen > 0) // crc & size are not part of the message, but only protocol.
                {
                    messageData = new byte[datalen];

                    int result2 = 0;
                    while (result2 < datalen)
                        result2 += server.Receive(messageData, result2, datalen - result2, SocketFlags.None);

                    if (result2 != datalen)
                        log.Items.Add("Réception incomplète de données de Rays Server");
                }
            }
            catch (SocketException e)
            {
                log.Items.Add("Erreur de reception de message de Rays Server: " + e.Message);
            }

            return (result == len);
        }

        private Socket server = null;
        private ListBox log = null;
        private ulong session_id = 0;
    }
}
