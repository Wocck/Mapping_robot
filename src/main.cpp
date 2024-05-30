#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "msg.h"
#include "periph/uart.h"
#include "ringbuffer.h"
#include "shell.h"
#include "stdio_uart.h"
#include "thread.h"
#include "xtimer.h"

#include "include/bluetoothUART.hpp"
#include "include/devicesConfig.hpp"
#include "include/dualMotorController.hpp"
#include "include/lidarUART.hpp"
#include "include/motorDriver.hpp"
#include "include/positionTracker.hpp"
#include "include/pulserDriver.hpp"

#ifndef SHELL_BUFSIZE
#define SHELL_BUFSIZE (128U)
#endif

void *__dso_handle __attribute__((weak)) = NULL;

void button_callback(void *arg) {
  (void)arg;

  puts("Startujemy ...\r\n");
}

BluetoothUART *btUART = nullptr;
LidarUART *liUART = nullptr;
MotorDriver *motorR = nullptr;
MotorDriver *motorL = nullptr;
PulserDriver *pulserR = nullptr;
PulserDriver *pulserL = nullptr;
DualMotorController *motController = nullptr;

static int cmd_send(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: %s <data (string)>\r\n", argv[0]);
    return 1;
  }

  btUART->send(argv[1]);
  return 0;
}

static const shell_command_t shell_commands[] = {
    {"send", "Send a string through given UART device", cmd_send},
    {NULL, NULL, NULL}};

int main(void) {
  printf("\r\n===================================\r\n");
  printf("UART INFO:\r\n");
  printf("Available devices:               %i\r\n", UART_NUMOF);
  if (STDIO_UART_DEV != UART_UNDEF) {
    printf("UART used for STDIO (the shell): UART_DEV(%i)\r\n", STDIO_UART_DEV);
  }

  motorR = new MotorDriver(&(devicesConfig::motorRight));
  motorL = new MotorDriver(&(devicesConfig::motorLeft));
  pulserR = new PulserDriver(&(devicesConfig::pulserRight));
  pulserL = new PulserDriver(&(devicesConfig::pulserLeft));
  motController = new DualMotorController(motorR, motorL, pulserL, pulserR, 20);

  btUART = new BluetoothUART(UART_DEV(1), 115200, motController, nullptr);
  liUART = new LidarUART(UART_DEV(2), 230400, nullptr);
  btUART->setLidarUART(liUART);
  liUART->setBluetoothUART(btUART);
  liUART->init();
  btUART->init();
  // liUART->startReceiving();

  char line_buf[SHELL_BUFSIZE];
  shell_run(shell_commands, line_buf, SHELL_BUFSIZE);
  return 0;
}