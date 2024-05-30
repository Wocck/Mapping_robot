// lidarUART.hpp
#ifndef LIDAR_UART_HPP
#define LIDAR_UART_HPP

#include <stddef.h>
#include <stdint.h>
#include "ringbuffer.h"
#include "periph/uart.h"
#include "xtimer.h"

#include "bluetoothUART.hpp"

#ifndef UART_BUFSIZE
#define UART_BUFSIZE        (128U)
#endif
#define DATARECEIVER_PRIO   (THREAD_PRIORITY_MAIN - 1)

#define RECEIVE_TIME_MS     (4000)

// Forward declaration of BluetoothUART
class BluetoothUART;

typedef struct {
    char rx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
} li_uart_ctx_t;

class LidarUART {
public:
    LidarUART(uart_t dev, uint32_t baud, BluetoothUART* bluetoothUART );
    void init();
    void setBluetoothUART(BluetoothUART *bluetoothUART);
    void startReceiving();
    void stopReceiving();

private:
    uart_t _dev;
    uint32_t _baud;
    BluetoothUART* _bluetoothUART;
    volatile bool _shouldTerminate = false;
    static li_uart_ctx_t _ctx[UART_NUMOF];
    static kernel_pid_t _dataReceiverPid;
    static char _dataReceiverThread[2048];
    static kernel_pid_t _commandProcessorPid; // Ensure it's declared as static

    xtimer_ticks32_t _startTime;
    
    static void rxCallback(void *arg, uint8_t data);
    static void *dataReceiveThread(void *arg);
    void processData(uint8_t *data, size_t len);
};

#endif // LIDAR_UART_HPP
