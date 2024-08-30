#include <Server.hpp>

Server::Server(void) {}

Server::Server(Tintin_reporter *tr) : _tintin_reporter(tr), _socket_fd(-1), _is_running(false)
{
	this->_tintin_reporter->log("Creating server", "INFO");
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_fd == -1)
	{
		this->_tintin_reporter->log("Failed to create socket", "ERROR");
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		this->_tintin_reporter->log("Failed to set socket options", "ERROR");
		close(this->_socket_fd);
		exit(EXIT_FAILURE);
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT);

	if (bind(this->_socket_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		this->_tintin_reporter->log("Failed to bind socket", "ERROR");
		close(this->_socket_fd);
		exit(EXIT_FAILURE);
	}
	if (listen(this->_socket_fd, MAX_CLIENTS) == -1)
	{
		this->_tintin_reporter->log("Failed to listen on socket", "ERROR");
		close(this->_socket_fd);
		exit(EXIT_FAILURE);
	}
	this->_pfds.push_back({this->_socket_fd, POLLIN, 0});
	this->_tintin_reporter->log("Server created", "INFO");
}

Server::~Server(void)
{
	this->shutdown_server();
}

void	Server::run_server(void)
{
	this->_is_running = true;
	while (this->_is_running)
	{
		int pcount = poll(this->_pfds.data(), this->_pfds.size(), -1);
		if (pcount == -1)
		{
			this->_tintin_reporter->log("poll() failed", "ERROR");
			break ;
		}

		for (size_t i = 0; i < this->_pfds.size(); ++i)
		{
			if (this->_pfds[i].revents & POLLIN)
			{
				if (this->_pfds[i].fd == this->_socket_fd)
				{
					sockaddr_in	client;
					socklen_t	client_len = sizeof(client);
					int client_fd = accept(this->_socket_fd, (struct sockaddr *)&client, &client_len);
					if (client_fd == -1)
					{
						this->_tintin_reporter->log("Failed to accept client connection", "ERROR");
						continue ;
					}
					if (this->_pfds.size() - 1 >= MAX_CLIENTS)
					{
						this->_tintin_reporter->log("Refused client connection", "INFO");
						send(client_fd, "Matt_daemon: Connection refused\r\n", 21, 0);
						close(client_fd);
						continue ;
					}
					this->_pfds.push_back({client_fd, POLLIN, 0});
					this->_tintin_reporter->log("Client accepted", "INFO");
				}
				else
					if (handle_client_input(this->_pfds[i].fd) == -1)
					{
						this->_tintin_reporter->log("Quitting", "INFO");
						return ;
					}
			}
		}
	}
}

void	Server::shutdown_server(void)
{
	if (this->_is_running)
	{
		this->_is_running = false;
		for (struct pollfd &pfd : this->_pfds)
			if (pfd.fd != this->_socket_fd)
				close(pfd.fd);
		this->_pfds.clear();
		if (this->_socket_fd != -1)
		{
			close(this->_socket_fd);
			this->_socket_fd = -1;
		}
	}
}

int	Server::handle_client_input(int client_socket)
{
	char	buffer[1024];

	memset(buffer, 0, sizeof(buffer));
	ssize_t	bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0)
	{
		this->remove_client(client_socket);
		close(client_socket);
		return 0;
	}

	std::string	message(buffer);
	message = message.substr(0, bytes_received - 1);
	if (message == "quit")
	{
		this->_tintin_reporter->log("Request quit", "INFO");
		this->shutdown_server();
		close(client_socket);
		return -1;
	}
	this->_tintin_reporter->log("User input: " + message, "LOG");
	return 0;
}

void Server::remove_client(int client_socket)
{
	std::vector<struct pollfd>::iterator it = std::remove_if(this->_pfds.begin(), this->_pfds.end(), [client_socket](pollfd &pfd) {
		return pfd.fd == client_socket;
	});
	if (it != this->_pfds.end())
		this->_pfds.erase(it);
}
