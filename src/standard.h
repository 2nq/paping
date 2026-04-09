#pragma once

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _CRT_SECURE_NO_WARNINGS 1
	#pragma comment(lib, "ws2_32.lib")
	// Include Windows headers first to avoid type conflicts
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#include <Windows.h>
	#include <time.h>
#endif

#define	VERSION	"2.0"
#define	AUTHOR	"Mike Lovell"
#define YEAR	((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

// snprintf is available in g++ natively — no redefinition needed

// Error codes
#define	SUCCESS						0
#define	ERROR_POUTOFMEMORY			100
#define	ERROR_SOCKET_CANNOTRESOLVE	101
#define	ERROR_SOCKET_TIMEOUT		102
#define	ERROR_SOCKET_GENERALFAILURE	103
#define	ERROR_SOCKET_REFUSED		104
#define	ERROR_INVALIDARGUMENTS		200


#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#ifndef WIN32	// Linux specific
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <unistd.h>
	#define closesocket close
#else
	#include "gettimeofday.h"
#endif

#include <sys/types.h>
#include <fcntl.h>

// Types
typedef	const		wchar_t*	pcw_t;
typedef	const		char*		pcc_t;
typedef				wchar_t*	pwc_t;
typedef				char*		pc_t;
typedef	unsigned	short		ushort_t;

#include "print.h"
#include "i18n.h"
#include "stats.h"
#include "timer.h"
#include "arguments.h"
#include "host.h"
#include "socket.h"


