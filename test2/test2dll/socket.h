#ifndef WIN32
#include <sys/select.h>
#else
#include <winsock2.h>
#endif

typedef class socket SOCK;

class socket
{
public:
	// shared or client variables
	int				descriptor;
	std::string		input;
	std::string		output;
	char			*ip;
// server variables (static, single instance)
	int				server;
	static fd_set	fd_read;
	static fd_set	fd_write;
	static fd_set	fd_exc;

	socket(int type, int protocol);
	socket(int desc);
	~socket();

	// shared or client functions
	int connect(char *address, int port);
	void disconnect();
	int get_address();
	int recv();
	int send();

	// server functions
	int accept();
	int check_connections();
	int listen();
	int open_port(int port);
	int start_server(int port);
};

