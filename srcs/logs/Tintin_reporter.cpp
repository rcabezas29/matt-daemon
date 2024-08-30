#include <Tintin_reporter.hpp>

Tintin_reporter::Tintin_reporter(void)
{
	struct stat	sb;
	if (stat("/var/log/matt_daemon", &sb) != 0 && mkdir("/var/log/matt_daemon", 0777) == -1)
	{
		std::cerr << "Error: Unable to crate log file" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->_log_file.open(LOG_FILE);
	this->log("Started", "INFO");
}

Tintin_reporter::~Tintin_reporter(void)
{
    this->_log_file.close();
}

void    Tintin_reporter::log(const std::string &message, const std::string &type)
{
	std::time_t currentTime = std::time(nullptr);
	std::tm		*localTime = std::localtime(&currentTime);

	this->_log_file << "[" << std::put_time(localTime, "%d / %m / %Y - %H : %M : %S") << "] ";
	this->_log_file << "[ " << type << " ] - ";
	this->_log_file << "Matt_daemon: ";
	this->_log_file << message << std::endl;
}
