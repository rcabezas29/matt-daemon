#pragma once

#define PORT 4242

class   Server
{
    public:
        Server(void);
        Server(const Server &copy);
        ~Server(void);
        Server &operator=(const Server &rhs);
};
