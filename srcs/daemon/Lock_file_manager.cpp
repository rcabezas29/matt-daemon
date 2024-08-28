#include <Lock_file_manager.hpp>

Lock_file_manager::Lock_file_manager(void) {}

Lock_file_manager::~Lock_file_manager(void) {}

void    Lock_file_manager::create_lock_file(void)
{
    this->_lffd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (this->_lffd == -1)
    {
        std::cerr << "Failed to open PID file." << std::endl;
        exit(1);
    }
    if (lockf(this->_lffd, F_TLOCK, 0) == -1)
    {
       std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
       close(this->_lffd);
        // report in logs
        exit(1);
    }
}

void    Lock_file_manager::remove_lock_file(void)
{
    lockf(this->_lffd, F_ULOCK, 0);
    close(this->_lffd);
    std::remove(LOCK_FILE);
}
