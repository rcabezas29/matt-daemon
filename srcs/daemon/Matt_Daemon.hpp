#pragma once

#include <Lock_file_manager.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <cstdlib>


class   Matt_Daemon
{
    private:
        Lock_file_manager   _lfm;

    public:
        Matt_Daemon(void);
        Matt_Daemon(const Matt_Daemon &copy);
        ~Matt_Daemon(void);
        Matt_Daemon &operator=(const Matt_Daemon &rhs);
        void    start_daemon(void);
        void    start_server(void);
};
