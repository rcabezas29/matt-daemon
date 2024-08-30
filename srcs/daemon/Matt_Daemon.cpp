#include <Matt_Daemon.hpp>

Matt_Daemon::Matt_Daemon(void) : _lfm(), _tintin_reporter(), _server(&_tintin_reporter) {}

Matt_Daemon::~Matt_Daemon(void) {}

void	Matt_Daemon::start_server(void)
{
	this->_server.run_server();
}

void    Matt_Daemon::start_daemon(void)
{
	pid_t   pid;

	this->_tintin_reporter.log("Entering Daemon mode", "INFO");
	pid = fork();
	if (pid < 0)
	{
		this->_tintin_reporter.log("Failed to daemonize", "ERROR");
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (setsid() < 0)
		exit(EXIT_FAILURE);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	pid = fork();
	if (pid < 0)
	{
		this->_tintin_reporter.log("Failed to daemonize", "ERROR");
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	chdir("/");
	this->_tintin_reporter.log("started. PID: "+ std::to_string(getpid()), "INFO");
}
