#include <Server.hpp>

Server::Server(void) {}

Server::Server(Tintin_reporter *tr) : _tintin_reporter(tr), _socket_fd(-1), _is_running(false)
{
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_fd == -1)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		perror("Failed to set socket options");
		close(this->_socket_fd);
		exit(EXIT_FAILURE);
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT);

	if (bind(this->_socket_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("Failed to bind socket");
		close(this->_socket_fd);
		exit(EXIT_FAILURE);
	}
	if (listen(this->_socket_fd, MAX_CLIENTS) == -1)
	{
		perror("Failed to listen on socket");
		close(this->_socket_fd);
		exit(EXIT_FAILURE);
	}
	this->_pfds.push_back({this->_socket_fd, POLLIN, 0});
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
			perror("poll() failed");
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
						perror("Failed to accept client connection");
						continue ;
					}
					if (this->_pfds.size() - 1 >= MAX_CLIENTS)
					{
						close(client_fd);
						continue ;
					}
					this->_pfds.push_back({client_fd, POLLIN, 0});
					std::cout << "Client connected." << std::endl;
				}
				else
					handle_client_input(this->_pfds[i].fd);
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

void Server::handle_client_input(int client_socket)
{
	char	buffer[1024];

	memset(buffer, 0, sizeof(buffer));
	ssize_t	bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytesReceived <= 0)
	{
		this->remove_client(client_socket);
		close(client_socket);
		return ;
	}

	std::string	message(buffer);
	message = message.substr(0, bytesReceived);
	if (message == "quit")
	{
		this->shutdown_server();
		close(client_socket);
		return ;
	}

	// Log the message (this would ideally go through TintinReporter)
	std::cout << "Received message: " << message << std::endl;
}

void Server::remove_client(int client_socket)
{
	std::vector<struct pollfd>::iterator it = std::remove_if(this->_pfds.begin(), this->_pfds.end(), [client_socket](pollfd &pfd) {
		return pfd.fd == client_socket;
	});
	if (it != this->_pfds.end())
		this->_pfds.erase(it);
}
