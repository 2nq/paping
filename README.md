# paping

TCP port ping utility. Continuously connects to a host:port and reports latency, with ASN lookup on startup.

## Usage

```
paping [options] destination
```

**Options:**

| Flag | Description |
|------|-------------|
| `-p, --port N` | TCP port to connect to (required) |
| `-c, --count N` | Number of probes (default: continuous) |
| `-t, --timeout N` | Timeout in milliseconds (default: 1000) |
| `--ip-bind` | Bind to a specific source IP |
| `--nocolor` | Disable colored output |
| `-?, --help` | Show usage |

## Example

```
paping 1.1.1.1 -p 443 -c 5
```

```
paping v1.5.5 - Copyright (c) 2026 Mike Lovell

Connecting to 1.1.1.1 on TCP 443:

Connected to 1.1.1.1 [AS13335 Cloudflare, Inc.] time=8.00ms protocol=TCP port=443
Connected to 1.1.1.1 [AS13335 Cloudflare, Inc.] time=8.97ms protocol=TCP port=443
Connected to 1.1.1.1 [AS13335 Cloudflare, Inc.] time=9.21ms protocol=TCP port=443
Connected to 1.1.1.1 [AS13335 Cloudflare, Inc.] time=8.51ms protocol=TCP port=443
Connected to 1.1.1.1 [AS13335 Cloudflare, Inc.] time=8.78ms protocol=TCP port=443

Connection statistics:
        Attempted = 5, Connected = 5, Failed = 0 (0.00%)
Approximate connection times:
        Minimum = 8.00ms, Maximum = 9.21ms, Average = 8.89ms
```

The ASN and organization name are fetched once at startup via [ip-api.com](http://ip-api.com) and displayed on every line. If the lookup fails, output falls back to the standard format.

## Building

**Windows (MinGW):**
```
g++ -o paping.exe -DWIN32 -std=c++11 src/main.cpp src/socket.cpp src/host.cpp src/print.cpp src/stats.cpp src/timer.cpp src/i18n.cpp src/arguments.cpp -x c++ src/gettimeofday.c -lws2_32 -D_POSIX_C_SOURCE=0 -U__STRICT_ANSI__
```

**Windows (Visual Studio):**  
Open `paping_vs2010/paping_vs2010.sln`

**Linux:**
```
make
```
