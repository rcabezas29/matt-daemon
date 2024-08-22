#pragma once

class   Signal_handler
{
    public:
        Signal_handler(void);
        Signal_handler(const Signal_handler &copy);
        ~Signal_handler(void);
        Signal_handler &operator=(const Signal_handler &rhs);
};
