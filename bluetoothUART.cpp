#include "include/bluetoothUART.hpp"
#include "msg.h"
#include "thread.h"
#include "xtimer.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>

const char *BluetoothUART::_endline = "\r\n";
uart_ctx_t BluetoothUART::_ctx[UART_NUMOF];
kernel_pid_t BluetoothUART::_commandProcessorPid;
char BluetoothUART::_commandProcessorThread[THREAD_STACKSIZE_MAIN];

BluetoothUART::BluetoothUART(uart_t dev, uint32_t baud,
                             DualMotorController *dualMotorController,
                             LidarUART *lidarUART)
    : _dev(dev), _baud(baud), _dualMotorController(dualMotorController),
      _lidarUART(lidarUART) {}

void BluetoothUART::init() {
  int res = uart_init(_dev, _baud, rxCallback, (void *)(intptr_t)_dev);
  if (res == UART_NOBAUD) {
    printf("Error: Given baudrate (%u) not possible\r\n", (unsigned int)_baud);
  } else if (res != UART_OK) {
    puts("Error: Unable to initialize UART device\r\n");
  } else {
    printf("Success: Initialized UART_DEV(%i) at BAUD %u\r\n", _dev,
           (unsigned int)_baud);
    for (unsigned i = 0; i < UART_NUMOF; i++) {
      ringbuffer_init(&(_ctx[i].rx_buf), _ctx[i].rx_mem, UART_BUFSIZE);
    }
    _commandProcessorPid =
        thread_create(_commandProcessorThread, sizeof(_commandProcessorThread),
                      COMMANDPROCESSOR_PRIO, 0, commandProcessorThread, this,
                      "commandProcessor");
  }
}

void BluetoothUART::send(const char *data) {
  uart_write(_dev, (uint8_t *)data, strlen(data));
}

void BluetoothUART::send(const uint8_t *data, size_t len) {
  uart_write(_dev, data, len);
}

void BluetoothUART::send(uint8_t data) {
  uart_write(_dev, &data, 1);
}


void BluetoothUART::_writeNewline() {
  uart_write(_dev, (uint8_t *)_endline, strlen(_endline));
}

void BluetoothUART::motorRun(uint16_t power, int16_t direction) {
  _dualMotorController->forward(power, direction);
}

void BluetoothUART::motorTurn(uint16_t degrees, uint16_t power) {
  _dualMotorController->rotateL(degrees, power);
}

void BluetoothUART::motorStop() { _dualMotorController->stop(); }

void BluetoothUART::motorRunTimed(uint16_t power, int16_t direction, uint16_t timeMs) {
  _dualMotorController->forward(power, direction);
  xtimer_msleep(timeMs);
  _dualMotorController->stop();
}

void BluetoothUART::lidarStartReceiving() { _lidarUART->startReceiving(); }

void BluetoothUART::lidarStopReceiving() { _lidarUART->stopReceiving(); }

void BluetoothUART::rxCallback(void *arg, uint8_t data) {
  uart_t dev = (uart_t)(uintptr_t)arg;

  ringbuffer_add_one(&_ctx[dev].rx_buf, data);

  if (data == '\n') {
    msg_t msg;
    msg.content.value = (uint32_t)dev;
    msg_send(&msg, _commandProcessorPid);
  }
}

void *BluetoothUART::commandProcessorThread(void *arg) {
  BluetoothUART *instance = static_cast<BluetoothUART *>(arg);
  msg_t msg;
  msg_t msg_queue[8];
  msg_init_queue(msg_queue, 8);

  while (1) {
    msg_receive(&msg);
    uart_t dev = (uart_t)msg.content.value;
    char c;
    char command[UART_BUFSIZE];
    unsigned int index = 0;

    do {
      c = (int)ringbuffer_get_one(&(_ctx[dev].rx_buf));
      if (c == '\n') {
        break;
      } else if (c == '\r') {
        // skip \r
      } else {
        command[index++] = c;
      }
    } while (index < UART_BUFSIZE - 1);
    command[index] = '\0';

    char *token = strtok(command, " ");
    if (token != NULL) {
      if (strcmp(token, "RUNTIMED") == 0) {
        instance->send("OK - RUNTIMED\r\n");
        // Extract parameters
        token = strtok(NULL, " ");
        uint16_t power = atoi(token);
        token = strtok(NULL, " ");
        int16_t direction = atoi(token);
        token = strtok(NULL, " ");
        uint16_t time = atoi(token);
        instance->motorRunTimed(power, direction, time);
      } else if (strcmp(token, "RUN") == 0) {
        instance->send("OK - RUN\r\n");
        // Extract parameters
        token = strtok(NULL, " ");
        uint16_t power = atoi(token);
        token = strtok(NULL, " ");
        int16_t direction = atoi(token);
        instance->motorRun(power, direction);
      } else if (strcmp(token, "TURN") == 0) {
        instance->send("OK - TURN\r\n");
        // Extract parameters
        token = strtok(NULL, " ");
        uint16_t degrees = atoi(token);
        token = strtok(NULL, " ");
        uint16_t power = atoi(token);
        instance->motorTurn(degrees, power);
      } else if (strcmp(token, "STOP") == 0) {
        instance->send("OK - STOP\r\n");
        instance->motorStop();
      } else if (strcmp(token, "LIDAR_START") == 0) {
        instance->send("OK - LIDAR_START\r\n");
        instance->lidarStartReceiving();
      } else if (strcmp(token, "LIDAR_STOP") == 0) {
        instance->send("OK - LIDAR_STOP\r\n");
        instance->lidarStopReceiving();
      } else if (strcmp(token, "CHECK_CONNECTION") == 0) {
        instance->send("OK\r\n");
      } else if (strcmp(token, "CHECK_PARAMS") == 0) {
        token = strtok(NULL, " ");
        char* param1 = token;
        token = strtok(NULL, " ");
        char* param2 = token;
        instance->send("PARAM1 = ");
        instance->send(param1);
        instance->send("\r\n");
        instance->send("PARAM2 = ");
        instance->send(param2);
        instance->send("\r\n");
        instance->send("OK\r\n");
      } else {
        instance->send("ERR\r\n");
      }
    }

    index = 0;
  }

  return NULL;
}

void BluetoothUART::setLidarUART(LidarUART *lidarUART) {
  _lidarUART = lidarUART;
}
