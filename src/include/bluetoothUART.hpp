#ifndef BLUETOOTH_UART_HPP
#define BLUETOOTH_UART_HPP

#include <stddef.h>
#include <stdint.h>

#include "periph/uart.h"
#include "ringbuffer.h"


#include "dualMotorController.hpp"
#include "lidarUART.hpp"

#ifndef UART_BUFSIZE
#define UART_BUFSIZE (128U-2)
#endif

#define COMMANDPROCESSOR_PRIO (THREAD_PRIORITY_MAIN - 1)

// Forward declaration of LidarUART
class LidarUART;

typedef struct {
  char rx_mem[UART_BUFSIZE];
  ringbuffer_t rx_buf;
} uart_ctx_t;

class BluetoothUART {
public:
  BluetoothUART(uart_t dev, uint32_t baud,
                DualMotorController *dualMotorController, LidarUART *lidarUART);
  void init();
  void send(const char *data);
  void send(const uint8_t *data, size_t len);
  void send(uint8_t data);
  static void rxCallback(void *arg, uint8_t data);
  static void *commandProcessorThread(void *arg);
  void setLidarUART(LidarUART *lidarUART);

private:
  static const char *_endline;
  uart_t _dev;
  uint32_t _baud;

  static uart_ctx_t _ctx[UART_NUMOF];
  static kernel_pid_t _commandProcessorPid;
  static char _commandProcessorThread[THREAD_STACKSIZE_MAIN];

  DualMotorController *_dualMotorController;
  LidarUART *_lidarUART;

  void _writeNewline();
  void motorRun(uint16_t power, int16_t direction);
  void motorTurn(uint16_t degrees, uint16_t power);
  void motorStop();
  void motorRunTimed(uint16_t power, int16_t direction, uint16_t timeMs);

  void lidarStartReceiving();
  void lidarStopReceiving();
};

#endif // BLUETOOTH_UART_HPP
