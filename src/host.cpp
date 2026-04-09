#include "standard.h"


using namespace std;


pcc_t host_c::IPAddressString()
{
	return host_c::GetIPAddressAsString(this->IPAddress);
}


int	host_c::GetSuccessfulConnectionString(pcc_t str, double time)
{
	int		length	= 0;

	if (this->ASNInfo[0] != '\0')
	{
		pcc_t format = i18n_c::GetString(STRING_CONNECT_SUCCESS_ASN);
		length = snprintf(NULL, 0, format, this->IPAddressString(), this->ASNInfo, time, socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->IPAddressString(), this->ASNInfo, time, socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}
	else
	{
		pcc_t format = i18n_c::GetString(STRING_CONNECT_SUCCESS);
		length = snprintf(NULL, 0, format, this->IPAddressString(), time, socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->IPAddressString(), time, socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}

	return length;
}


void host_c::LookupASN(const char* ip, char* buf, int buflen)
{
	buf[0] = '\0';

#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;
#endif

	struct addrinfo hints = {}, *res = NULL;
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo("ip-api.com", "80", &hints, &res) != 0 || !res)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return;
	}

	sockaddr_in addr = *(sockaddr_in*)res->ai_addr;
	freeaddrinfo(res);

	int sock = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return;
	}

#ifdef WIN32
	DWORD tv_ms = 3000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_ms, sizeof(tv_ms));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv_ms, sizeof(tv_ms));
#else
	struct timeval tv; tv.tv_sec = 3; tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif

	if (connect(sock, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		closesocket(sock);
#ifdef WIN32
		WSACleanup();
#endif
		return;
	}

	char request[256];
	snprintf(request, sizeof(request),
		"GET /line/%s?fields=as HTTP/1.0\r\nHost: ip-api.com\r\nConnection: close\r\n\r\n", ip);
	send(sock, request, (int)strlen(request), 0);

	char response[1024];
	int total = 0, n = 0;
	while (total < (int)sizeof(response) - 1)
	{
		n = recv(sock, response + total, (int)(sizeof(response) - 1 - total), 0);
		if (n <= 0) break;
		total += n;
	}
	closesocket(sock);
#ifdef WIN32
	WSACleanup();
#endif

	if (total <= 0) return;
	response[total] = '\0';

	// Skip HTTP headers
	char* body = strstr(response, "\r\n\r\n");
	if (!body) return;
	body += 4;

	// Trim trailing whitespace/newlines
	int len = (int)strlen(body);
	while (len > 0 && (body[len-1] == '\r' || body[len-1] == '\n' || body[len-1] == ' '))
		body[--len] = '\0';

	if (len > 0 && len < buflen)
		memcpy(buf, body, len + 1);
}


int host_c::GetConnectInfoString(pcc_t str)
{
	int		length	= 0;
	pcc_t	format	= NULL;
	
	if (this->HostIsIP)
	{
		format = i18n_c::GetString(STRING_CONNECT_INFO_IP);

		length = snprintf(NULL, 0, format, this->Hostname, socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->Hostname, socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}
	else
	{
		format = i18n_c::GetString(STRING_CONNECT_INFO_FULL);
		
		length = snprintf(NULL, 0, format, this->Hostname, this->IPAddressString(), socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->Hostname, this->IPAddressString(), socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}
	
	
	return length;
}


pcc_t host_c::GetIPAddressAsString(in_addr ipAddress)
{
	return inet_ntoa(ipAddress);
}
