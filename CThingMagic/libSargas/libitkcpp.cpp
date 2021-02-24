#include "libitkcpp.h"

Libitkcpp::Libitkcpp()
{

}

Libitkcpp::~Libitkcpp() {

}

void Libitkcpp::onSendEvent(string method, string message) {
    LOGD("onSendEvent", "method: %s, message: %s", method.c_str(), message.c_str());
}
