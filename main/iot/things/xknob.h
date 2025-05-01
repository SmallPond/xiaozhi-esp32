#ifndef __XKNOB_H__
#define __XKNOB_H__

#include "iot/communication/simple_comm.h"
#include "sdkconfig.h"
#include "iot/thing.h"
#include <esp_log.h>

namespace iot {

class XKnob : public Thing {
public:
    XKnob(SimpleComm *comm);
    void SendCommand(const std::string& cmd);
private:
    SimpleComm *comm_;
};

} // namespace iot

#endif // __XKNOB_H__