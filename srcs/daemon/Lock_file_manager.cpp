#include <Lock_file_manager.hpp>

Lock_file_manager::Lock_file_manager(void)
{
	this->_lffd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
	if (this->_lffd == -1)
	{
		std::cerr << "Failed to open PID file." << std::endl;
		exit(1);
	}
	if (flock(this->_lffd, LOCK_EX | LOCK_NB) != 0)
	{
		std::cerr << "Failed to acquire lock, another instance might be running." << std::endl;
		close(this->_lffd);
		// report in logs
		exit(1);
	}
}

Lock_file_manager::~Lock_file_manager(void)
{
	this->remove_lock_file();
}

void    Lock_file_manager::remove_lock_file(void)
{
	flock(this->_lffd, LOCK_UN);
	close(this->_lffd);
	std::remove(LOCK_FILE);
}
