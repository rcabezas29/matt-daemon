#pragma once

#define PORT 4242
#define MAX_CLIENTS 3

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <csignal>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <algorithm>

#include <Tintin_reporter.hpp>

class Server
{
	private:
		std::vector<pollfd>			_pfds;
		Tintin_reporter				*_tintin_reporter;
		int							_socket_fd;
		bool						_is_running;

		Server(void);
		Server(const Server &copy);
		Server	&operator=(const Server &rhs);
		int		handle_client_input(int clientSocket);
		void	remove_client(int clientSocket);

	public:
		Server(Tintin_reporter *tr);
		~Server(void);

		void	run_server(void);
		void	shutdown_server(void);
		void	remove_clients(void);
};
