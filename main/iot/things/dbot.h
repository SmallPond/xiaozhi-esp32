#ifndef DBOT_H
#define DBOT_H

#include "iot/communication/simple_comm.h"
#include <memory>
#include "sdkconfig.h"
#include "iot/thing.h"
#include <esp_log.h>

namespace iot {

class DBot : public Thing {
public:
    DBot(SimpleComm *comm);
    void SendCommand(const std::string& cmd);

private:
    SimpleComm* comm_;
};

} // namespace iot

#endif // DBOT_H