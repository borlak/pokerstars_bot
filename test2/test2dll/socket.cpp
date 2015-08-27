#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef WIN32
	#include <winsock2.h>
	#include <vector>
	#include "llist.h"
	#include "util.h"
	typedef char raw_type;
	typedef int socklen_t;
#else
	#include <string.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include "llist.h"
	typedef void raw_type;
	using namespace std;
	extern void Log(const char *str, ...);
#endif

#include "socket.h"

#ifdef WIN32
	static bool initialized = false;
#endif

fd_set	socket::fd_read;
fd_set	socket::fd_write;
fd_set	socket::fd_exc;
llist<SOCK> sockets;

// Function to fill in address structure given an address and port
void fill_address(const char *address, int port, sockaddr_in &addr)
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	hostent *host;  // Resolve name
	if((host = ::gethostbyname(address)) == NULL)
		Log("socekt: Failed to resolve name gethostbyname()");

	addr.sin_addr.s_addr	= *((unsigned long *) host->h_addr_list[0]);
	addr.sin_port			= htons(port);
}

socket::socket(int type, int protocol)
{
	::memset(this, 0, sizeof(SOCK));
#ifdef WIN32
	if(!initialized)
	{
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(2, 0);
		if (::WSAStartup(wVersionRequested, &wsaData) != 0)
			Log("socket: Unable to startup winsock");
		initialized = true;
	}
#endif
	// Make a new socket
	if((descriptor = ::socket(AF_INET, type, protocol)) < 0)
		Log("socket: Unable to create socket");
}

socket::socket(int desc)
{
	::memset(this, 0, sizeof(SOCK));

	descriptor	= desc;
	output		= "";
	input		= "";
}

socket::~socket()
{
#ifdef WIN32
	if(server)
	{
		::WSACleanup();
		Log("socket: host: closing server...");
	}
	::closesocket(descriptor);
	Log("socket: closing descriptor %d, ip[%s]", descriptor, ip);
#else
	::close(descriptor);
#endif
	descriptor = -1;

	if(ip)		delete [] ip;
}

int socket::open_port(int port)
{
	sockaddr_in localAddr;

	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	localAddr.sin_port			= htons(port);

	if(::bind(descriptor, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0)
	{
		Log("socket: Set of local port failed bind()");
		return 0;
	}
	return 1;
}

int socket::accept()
{
	SOCK *sock;
	int new_desc;

#ifndef WIN32
	if((new_desc = ::accept(descriptor, NULL, 0)) < 0)
#else
	if((new_desc = ::accept(descriptor, NULL, 0)) == INVALID_SOCKET)
#endif
	{
#ifndef WIN32
		Log("socket: Accept failed accept()");
#else
		Log("socket: Accept failed accept() Error [%d]", WSAGetLastError());
#endif
		return 0;
	}

	sock = new SOCK(new_desc);
	sock->get_address();
	Log("socket: New connection from %s", sock->ip);
	sockets.insert(sock);
	return 1;
}

int socket::listen()
{
	if(::listen(descriptor, 0) < 0)
	{
		Log("socket: Set listening socket failed listen()");
		return 0;
	}
	return 1;
}

int socket::connect(char *address, int port)
{
	// Get the address of the requested host 
	sockaddr_in dest_addr;
	fill_address(address, port, dest_addr);

	// Try to connect to the given port
	if(::connect(descriptor, (sockaddr *) &dest_addr, sizeof(sockaddr_in)) < 0)
	{
		Log("socket: Connect failed connect()");
		return 0;
	}
	Log("socket: Connected to %s port %d", address, port);
	return 1;
}

int socket::send()
{
	if(::send(descriptor, (raw_type *)output.c_str(), output.length(), 0) < 0)
	{
		Log("socket: Send failed send()");
		return 0;
	}
	output.clear();
	return 1;
}

int socket::recv()
{
	int bytes=0, total=0;
	char buf[513];

	do {
		bytes = ::recv(descriptor, (raw_type *)buf, 512, 0);
		if(bytes > 0)
		{
			total += bytes;
			buf[bytes] = '\0';
Log("socket: received [%s] from %d\n", buf, descriptor);
			input += buf;
		}
		else if(bytes == 0)
			break;
		else
		{
#ifdef WIN32
			Log("socket: recv: error %d", ::WSAGetLastError());
#else
			Log("socket: recv: error");
#endif		
			break;
		}
	} while(bytes > 0);

	return total;
}

int socket::start_server(int port)
{
	if(open_port(port) && listen())
	{
		Log("socket: Server started on port %d", port);
		server = 1;
		return 1;
	}
	return 0;
}

int socket::get_address()
{
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);
	char *ip_str;

	if(getpeername(descriptor, (sockaddr *)&addr, (socklen_t *) &addr_len) < 0)
	{
		Log("socket: Fetch of foreign address failed getpeername()");
		return 0;
	}

	if((ip_str = inet_ntoa(addr.sin_addr)))
	{
		ip = new char[strlen(ip_str)+1];
		strcpy(ip, ip_str);
	}
	return 1;
}


int socket::check_connections()
{
	SOCK *sock, *sock_next;
	struct timeval tv;
	int max_desc = descriptor;
	int desc;

	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_exc);
	FD_SET(descriptor, &fd_read);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	for(sock = sockets.begin(); sock; sock = sockets.next())
	{
		desc = sock->descriptor;

		if(desc > max_desc)
			max_desc = desc;

		FD_SET(desc, &fd_read);
		FD_SET(desc, &fd_write);
		FD_SET(desc, &fd_exc);
	}

	if(::select(max_desc+1, &fd_read, &fd_write, &fd_exc, &tv) < 0)
		Log("socket: select()");

	if(FD_ISSET(descriptor, &fd_read))
		accept();

	for(sock = sockets.begin(); sock; sock = sock_next)
	{
		sock_next = sockets.next();

		desc = sock->descriptor;

		if(FD_ISSET(desc, &fd_exc))
		{
			sockets.remove();
			delete sock;
			continue;
		}

		if(FD_ISSET(desc, &fd_write) && sock->output.length() > 0)
		{
Log("socket: check connections: found data....");
			sock->send();
		}

		if(FD_ISSET(desc, &fd_read))
		{
			if(sock->recv() <= 0)
			{
				sockets.remove();
				delete sock;
				continue;
			}
		}
	}
	return 0;
}


