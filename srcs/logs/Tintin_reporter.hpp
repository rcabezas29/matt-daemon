#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>

#define LOG_FILE "/var/log/matt_daemon/matt_daemon.log"

class	Tintin_reporter
{
	private:
		std::ofstream	_log_file;

		Tintin_reporter(const Tintin_reporter &copy);
		Tintin_reporter	&operator=(const Tintin_reporter &rhs);

	public:
		Tintin_reporter(void);
		~Tintin_reporter(void);

		void    log(const std::string &message, const std::string &type);
};
