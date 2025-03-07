#ifndef __UART_COMM_H__
#define __UART_COMM_H__

#include <driver/uart.h>
#include "simple_comm.h"

namespace iot {

class UARTComm : public SimpleComm {
public:
    UARTComm(uart_port_t port = UART_NUM_1, 
                  int tx_pin = 0,
                  int rx_pin = 0,
                  int baud_rate = 115200) 
        : port_(port), tx_pin_(tx_pin), rx_pin_(rx_pin), baud_rate_(baud_rate) {}

    int Init() override {
        uart_config_t config = {
            .baud_rate = baud_rate_,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };

        if (is_inited_) {
            return 0;
        }

        ESP_ERROR_CHECK(uart_driver_install(port_, 1024*2, 0, 0, NULL, 0));
        ESP_ERROR_CHECK(uart_param_config(port_, &config));
        ESP_ERROR_CHECK(uart_set_pin(port_, tx_pin_, rx_pin_, -1, -1));

        is_inited_ = true;
        return 0;
    }

    int Send(const std::string& str) override {
        return uart_write_bytes(port_, str.c_str(), str.size()) == str.size();
    }

    // run in freertos task
    // void startReceiving() {
    //     xTaskCreate([](void* arg) {
    //         UARTComm* self = static_cast<UARTComm*>(arg);
    //         uint8_t buffer[128];
    //         while (true) {
    //             int len = uart_read_bytes(self->port_, buffer, sizeof(buffer),
    //                                             pdMS_TO_TICKS(100));
    //             if (len > 0 && self->callback_) {
    //                 self->callback_({buffer, buffer + len});
    //             }
    //         }
    //     }, "uart_rx_task", 4096, this, 5, NULL);
    // }

    void SetRecvCallback(RecvCallback callback) override { callback_ = callback; }

private:
    uart_port_t port_;
    int tx_pin_, rx_pin_, baud_rate_;
    bool is_inited_ = false;
    RecvCallback callback_;
};

}; // namespace iot


#endif