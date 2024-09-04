#include <Server.hpp>

Server::Server(void) {}
Server::Server(const Server &copy) { (void)copy; }
Server	&Server::operator=(const Server &rhs) { (void)rhs; return *this; }

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
	this->_sfd = setup_signal_fd();
	if (this->_sfd == -1)
	{
		this->_tintin_reporter->log("Failed to set up signal fd", "ERROR");
		return;
	}
	this->_pfds.push_back({this->_sfd, POLLIN, 0});
}

Server::~Server(void)
{
	this->shutdown_server();
}

int	Server::setup_signal_fd(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGHUP);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
	{
        perror("sigprocmask");
        return -1;
    }
    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) {
        perror("signalfd");
        return -1;
    }
    return sfd;
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
				if (this->_pfds[i].fd == this->_sfd)
				{
					struct signalfd_siginfo fdsi;
					ssize_t	s = read(this->_sfd, &fdsi, sizeof(struct signalfd_siginfo));
					if (s != sizeof(struct signalfd_siginfo))
					{
						this->_tintin_reporter->log("signalfd read() failed", "ERROR");
						exit(1);
					}
					if (fdsi.ssi_signo == SIGHUP)
					{
						this->remove_clients();
						this->_tintin_reporter->log("Daemon reloaded", "SIGNAL (SIGHUP)");
					}
					else if (fdsi.ssi_signo == SIGTERM || fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGQUIT)
					{
						this->_shutdown_requested = 1;
						if (fdsi.ssi_signo == SIGTERM)
							this->_tintin_reporter->log("Daemon stopped", "SIGNAL (SIGTERM)");
						else if (fdsi.ssi_signo == SIGINT)
							this->_tintin_reporter->log("Daemon stopped", "SIGNAL (SIGINT)");
						else
							this->_tintin_reporter->log("Daemon stopped", "SIGNAL (SIGQUIT)");
						
					}
				}
				else if (this->_pfds[i].fd == this->_socket_fd)
				{
					sockaddr_in	client;
					socklen_t	client_len = sizeof(client);
					int client_fd = accept(this->_socket_fd, (struct sockaddr *)&client, &client_len);
					if (client_fd == -1)
					{
						this->_tintin_reporter->log("Failed to accept client connection", "ERROR");
						continue ;
					}
					if (this->_pfds.size() - 2 >= MAX_CLIENTS)
					{
						this->_tintin_reporter->log("Refused client connection", "INFO");
						send(client_fd, "Matt_daemon: Connection refused\r\n", 34, 0);
						close(client_fd);
						continue ;
					}
					this->_pfds.push_back({client_fd, POLLIN, 0});
					this->_tintin_reporter->log("Client accepted", "INFO");
				}
				else
				{
					if (handle_client_input(this->_pfds[i].fd) == -1)
					{
						this->_tintin_reporter->log("Quitting", "INFO");
						return ;
					}
				}
			}
		}
		if (this->_shutdown_requested)
		{
			this->shutdown_server();
			break ;
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
	message.erase(message.find_last_not_of("\r\n") + 1);
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
	for (size_t i = 2; i < MAX_CLIENTS + 2; ++i)
		if (this->_pfds[i].fd == client_socket)
			close(this->_pfds[i].fd);
	std::vector<struct pollfd>::iterator it = std::remove_if(this->_pfds.begin(), this->_pfds.end(), [client_socket](pollfd &pfd) {
		return pfd.fd == client_socket;
	});
	if (it != this->_pfds.end())
		this->_pfds.erase(it);
}

void	Server::remove_clients(void)
{
	for (size_t i = 2; i < this->_pfds.size(); ++i)
		close(this->_pfds[i].fd);
	this->_pfds.erase(this->_pfds.begin() + 2, this->_pfds.end());
}
