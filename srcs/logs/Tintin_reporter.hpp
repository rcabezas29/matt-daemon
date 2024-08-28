#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>

#define LOG_FILE "/var/log/matt_daemon/matt_daemon.log"

class   Tintin_reporter
{
    private:
        std::ofstream    _log_file;

    public:
        Tintin_reporter(void);
        Tintin_reporter(const Tintin_reporter &copy);
        ~Tintin_reporter(void);
        Tintin_reporter &operator=(const Tintin_reporter &rhs);

        void    log(const std::string &message);
        void    create_log_file(void);
};
