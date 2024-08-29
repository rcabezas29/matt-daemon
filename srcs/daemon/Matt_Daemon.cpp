#include <Matt_Daemon.hpp>

Matt_Daemon::Matt_Daemon(void) : _lfm() {}

Matt_Daemon::~Matt_Daemon(void) {}

void	Matt_Daemon::start_server(void)
{
	while (true)
	{
		sleep(5);
	}
}

void    Matt_Daemon::start_daemon(void)
{
    pid_t   pid;
    this->_lfm.create_lock_file();
    
    /* Fork off the parent process */
    pid = fork();
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
     /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    
    /* Catch, ignore and handle signals */
    /*TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    
    /* Fork off for the second time*/
    pid = fork();
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    
    /* Set new file permissions */
    umask(0);
    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");
    
    /* Open the log file */
    // openlog("firstdaemon", LOG_PID, LOG_DAEMON);
}