#ifndef LIBITKCPP_H
#define LIBITKCPP_H

#include "libsargascpp.h"

class Libitkcpp : public SargasEventListener
{
public:
    Libitkcpp();
    ~Libitkcpp();

    void onSendEvent(std::string method, std::string message);
};

#endif // LIBITKCPP_H
