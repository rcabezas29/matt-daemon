#pragma once

#include <Lock_file_manager.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <cstdlib>

#include <Server.hpp>

class   Matt_Daemon
{
    private:
        Lock_file_manager   _lfm;
		Tintin_reporter		_tintin_reporter;
		Server				_server;

		static Matt_Daemon		*_instance;

        Matt_Daemon(const Matt_Daemon &copy);
        Matt_Daemon &operator=(const Matt_Daemon &rhs);

    public:
        Matt_Daemon(void);
        ~Matt_Daemon(void);
        void    start_daemon(void);
        void    start_server(void);
};
