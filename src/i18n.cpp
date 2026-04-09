#include "standard.h"


using namespace std;


pcc_t i18n_c::GetString(int id)
{
	switch (id)
	{
		case STRING_USAGE:						return 	"Syntax: paping [options] destination\n\n"
														"Options:\n"
														" -?, --help\t\tdisplay usage\n"
														" -p, --port N\t\tset TCP port N (required)\n"
														" -c, --count N\t\tset number of checks to N\n"
														" -t, --timeout N\ttimeout in milliseconds (default 1000)\n"
														" -i, --interval N\tinterval between probes in milliseconds (default 1000)\n"
														"     --nocolor\t\tdisable color output\n"
														"     --ip-bind\t\tset source ip address\n";

		case STRING_STATS:						return	"\n"
														"Connection statistics:\n"
														"\tAttempted = $%d$, Connected = $%d$, Failed = $%d$ ($%.2f%%$)\n"
														"Approximate connection times:\n"
														"\tMinimum = $%.2fms$, Maximum = $%.2fms$, Average = $%.2fms$\n";

		case ERROR_POUTOFMEMORY:				return	"Out of memory";
		case ERROR_SOCKET_TIMEOUT:				return	"$Connection timed out$";
		case ERROR_SOCKET_REFUSED:				return	"$Connection refused$";
		case ERROR_SOCKET_GENERALFAILURE:		return	"$General failure$";
		case ERROR_SOCKET_CANNOTRESOLVE:		return	"$Cannot resolve host$";

		case STRING_CONNECT_INFO_FULL:			return	"Connecting to $%s$ [$%s$] on $%s %d$:";
		case STRING_CONNECT_INFO_IP:			return	"Connecting to $%s$ on $%s %d$:";
		case STRING_CONNECT_SUCCESS:			return	"Connected to $%s$: time=$%.2fms$ protocol=$%s$ port=$%d$";
		case STRING_CONNECT_SUCCESS_ASN:		return	"Connected to $%s$: time=$%.2fms$ protocol=$%s$ port=$%d$ | %s";
		default:								return	"[ERROR: No translation]";
	}
}
