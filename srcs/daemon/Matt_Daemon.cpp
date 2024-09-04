#include <Matt_Daemon.hpp>

Matt_Daemon *Matt_Daemon::_instance = nullptr;

Matt_Daemon::Matt_Daemon(const Matt_Daemon &copy) : _lfm(), _tintin_reporter(), _server(&_tintin_reporter) { (void)copy; }
Matt_Daemon &Matt_Daemon::operator=(const Matt_Daemon &rhs) { (void)rhs; return *this; }

Matt_Daemon::Matt_Daemon(void) : _lfm(), _tintin_reporter(), _server(&_tintin_reporter)
{
	this->_instance	= this;
}

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
