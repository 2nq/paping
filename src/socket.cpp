#include "standard.h"

using namespace std;

int socket_c::Resolve(pcc_t hostname, host_c &host)
{
	#ifdef WIN32	// Init Winsock in Windows
		WSADATA	wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return ERROR_SOCKET_GENERALFAILURE;
	#endif

	struct addrinfo hints = {}, *res = NULL;
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname, NULL, &hints, &res) != 0 || res == NULL)
	{
		#ifdef WIN32
			WSACleanup();
		#endif
		return ERROR_SOCKET_CANNOTRESOLVE;
	}

	host.IPAddress = ((sockaddr_in*)res->ai_addr)->sin_addr;
	host.Hostname  = hostname;
	host.HostIsIP  = (inet_addr(hostname) != INADDR_NONE);

	freeaddrinfo(res);

	#ifdef WIN32
		WSACleanup();
	#endif

	return SUCCESS;
}


int socket_c::SetPortAndType(int port, int type, host_c &host)
{
	host.Port	= port;
	host.Type	= type;

	return SUCCESS;
}
int socket_c::SetSource(int Source, host_c &host)
{
	host.Source	= htonl(Source);
	return SUCCESS;
}

pcc_t socket_c::GetFriendlyTypeName(int type)
{
	switch (type)
	{
		case IPPROTO_TCP:
			return "TCP";
		case IPPROTO_UDP:
			return "UDP";
		default:
			return "UNKNOWN";
	}
}


int socket_c::GetSocketType(int type)
{
	switch (type)
	{
		case IPPROTO_UDP:
			return SOCK_DGRAM;
		default:
			return SOCK_STREAM;
	}
}


int socket_c::Connect(host_c host, int timeout, double &time)
{
	int		result	= 0;
	int		clientSocket;


	#ifdef WIN32	// Init Winsock in Windows
		WSADATA	wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return ERROR_SOCKET_GENERALFAILURE;
	#endif

	clientSocket = socket(AF_INET, socket_c::GetSocketType(host.Type), host.Type);

	if (clientSocket == -1) return ERROR_SOCKET_GENERALFAILURE;

	sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = host.Source;
	localaddr.sin_port = 0;
	bind(clientSocket, (struct sockaddr *)&localaddr, sizeof(localaddr));

	sockaddr_in	clientAddress;

	clientAddress.sin_addr		= host.IPAddress;
	clientAddress.sin_family	= AF_INET;
	clientAddress.sin_port		= htons(host.Port);

	timeval	tv;
	
	// No blocking for Windows/Linux
	#ifdef WIN32
		ULONG	mode = 1;
		ioctlsocket(clientSocket, FIONBIO, &mode);
	#else
		long arg = fcntl(clientSocket, F_GETFL, host.Type);
		arg = fcntl(clientSocket, F_SETFL, arg | O_NONBLOCK);
	#endif


	tv.tv_sec 	= 0;
	tv.tv_usec	= timeout * 1000;

	timer_c	timer;

	timer.Start();		

	connect(clientSocket, (sockaddr*)&clientAddress, sizeof(clientAddress));

	fd_set	read, write;

	FD_ZERO(&read);
	FD_ZERO(&write);

	FD_SET(clientSocket, &read);
	FD_SET(clientSocket, &write);
	
	result = select(clientSocket + 1, &read, &write, NULL, &tv);

	if (result <= 0)
	{
		closesocket(clientSocket);

		#ifdef WIN32	// Cleanup Winsock in Windows
			WSACleanup();
		#endif

		return ERROR_SOCKET_TIMEOUT;
	}

	time = timer.Stop();

	// Check whether the connection actually succeeded
	int sockErr = 0;
	#ifdef WIN32
		int sockErrLen = sizeof(sockErr);
		getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, (char*)&sockErr, &sockErrLen);
	#else
		socklen_t sockErrLen = sizeof(sockErr);
		getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, &sockErr, &sockErrLen);
	#endif

	if (sockErr != 0)
	{
		closesocket(clientSocket);

		#ifdef WIN32
			WSACleanup();
		#endif

		return ERROR_SOCKET_REFUSED;
	}

	closesocket(clientSocket);

	#ifdef WIN32	// Cleanup Winsock in Windows
		WSACleanup();
	#endif

	return SUCCESS;
}
