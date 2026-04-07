#include "standard.h"

class host_c
{
	public:
		pcc_t	Hostname;
		in_addr	IPAddress;
		unsigned int Source;
		bool	HostIsIP;
		int		Port;
		int		Type;
		char	ASNInfo[128];

		pcc_t	IPAddressString();
		int		GetConnectInfoString(pcc_t str);
		int		GetSuccessfulConnectionString(pcc_t str, double time);

		static	pcc_t	GetIPAddressAsString(in_addr ipAddress);
		static	pcc_t	GetFriendlyTypeName(int type);
		static	void	LookupASN(const char* ip, char* buf, int buflen);
};
