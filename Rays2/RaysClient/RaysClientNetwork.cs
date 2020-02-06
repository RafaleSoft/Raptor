using System.Net;
using System.Net.Sockets;

namespace RaysClient
{
    class RaysClientNetwork
    {
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
                // TODO: log not supported address family (v6)
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

        private Socket server = null;
    }
}
