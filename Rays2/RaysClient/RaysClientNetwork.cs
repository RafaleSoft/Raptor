using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Windows.Forms;


namespace RaysClient
{
    class RaysClientNetwork
    {
        public struct MSGSTRUCT
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
        public const uint SESBASE = 0x01000000;
        public const uint SES_OPEN = SESBASE + 0x01;
        public const uint SES_CLOSE = SESBASE + 0x02;
        public const uint SES_ID = SESBASE + 0x03;

        // Job Messages semantic
        public const uint JOB_BASE = 0x00000000;
        public const uint JOB_ID = JOB_BASE + 0x01;
        public const uint JOB_START = JOB_BASE + 0x02;
        public const uint JOB_DATA = JOB_BASE + 0x0f;


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
                log.Warning("Addressage IPv6 non supporté: " + address);
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
                log.Warning("Echec de connection à Rays Server: " + e.Message);
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

        public void SetLog(ref RaysLogger l)
        {
            log = l;
        }

        public bool SendMessage(uint messageId, uint data0, uint data1, uint data2, uint data3, uint data4, ref byte[] messageData)
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
                log.Error("Erreur d'envoi de message à Rays Server: " + e.Message);
            }

            return (result == datalen);
        }

        public bool ReceiveMessage(ref MSGSTRUCT msg, ref byte[] messageData)
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
                        log.Error("Réception incomplète de données de Rays Server");
                }
            }
            catch (SocketException e)
            {
                log.Error("Erreur de reception de message de Rays Server: " + e.Message);
            }

            return (result == len);
        }

        private Socket server = null;
        private RaysLogger log = null;
    }
}
