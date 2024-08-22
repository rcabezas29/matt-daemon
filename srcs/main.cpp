#include <Matt_Daemon.hpp>

int main(void)
{
    Matt_Daemon md;

    if (getuid())
    {
        std::cerr << "Matt_daemon must be run with root rights" << std::endl;
		return 1;
    }

    md.start_daemon();
    md.start_server();

    return 0;
}
