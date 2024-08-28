#pragma once

#include <cstdio>
#include <sys/file.h>
#include <unistd.h>
#include <iostream>

#define LOCK_FILE "/var/lock/matt_daemon.lock"

class   Lock_file_manager
{
    private:
        int _lffd;

    public:
        Lock_file_manager(void);
        Lock_file_manager(const Lock_file_manager &copy);
        ~Lock_file_manager(void);
        Lock_file_manager &operator=(const Lock_file_manager &rhs);

        void    create_lock_file(void);
        void    remove_lock_file(void);
};
