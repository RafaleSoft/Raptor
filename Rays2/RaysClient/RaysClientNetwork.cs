using System;
using System.Net;
using System.Net.Sockets;

namespace RaysClient
{
    class RaysClientNetwork
    {
        public bool Connect(string address, int port)
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

            try
            {
                server.Connect(addr,port);
            }
            catch(SocketException e)
            {
                // TODO: log not supported address family (v6)
                return false;
            }

            return true;
        }

        public bool IsConnected()
        {
            return server.Connected;
        }

        private Socket server;
    }
}
