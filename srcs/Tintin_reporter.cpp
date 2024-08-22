#include <Tintin_reporter.hpp>

Tintin_reporter::Tintin_reporter(void) : _log_file(LOG_FILE) {}

Tintin_reporter::~Tintin_reporter(void)
{
    this->_log_file.close();
}

void    Tintin_reporter::log(const string &message)
{
    std::time_t currentTime = std::time(nullptr);
    std::tm     *localTime = std::localtime(&currentTime);

    this->_log_file << "[" << std::put_time(localTime, "%d / %m / %Y - %H : %M : %S") << "]";
    this->_log_file << message << std::endl;
}
