#include "include/lidarUART.hpp"
#include "msg.h"
#include "thread.h"
#include "xtimer.h"
#include <algorithm>
#include <stdio.h>
#include <string.h>

li_uart_ctx_t LidarUART::_ctx[UART_NUMOF];
kernel_pid_t LidarUART::_dataReceiverPid;
char LidarUART::_dataReceiverThread[2048];
kernel_pid_t LidarUART::_commandProcessorPid;

LidarUART::LidarUART(uart_t dev, uint32_t baud, BluetoothUART *bluetoothUART)
    : _dev(dev), _baud(baud), _bluetoothUART(bluetoothUART) {}

void LidarUART::init() {
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
  }
}

void LidarUART::setBluetoothUART(BluetoothUART *bluetoothUART) {
  _bluetoothUART = bluetoothUART;
}
void LidarUART::rxCallback(void *arg, uint8_t data) {
  uart_t dev = (uart_t)(uintptr_t)arg;

  ringbuffer_add_one(&_ctx[dev].rx_buf, data);
  msg_t msg;
  msg.content.value = (uint32_t)dev;
  msg_send(&msg, _dataReceiverPid);
}

void *LidarUART::dataReceiveThread(void *arg) {
  LidarUART *instance = static_cast<LidarUART *>(arg);
  msg_t msg;
  msg_t msg_queue[8];
  msg_init_queue(msg_queue, 8);

  uint8_t data[47];
  unsigned int data_index = 0;
  bool packet_started = false;

  instance->_startTime = xtimer_now_usec();
  while (!instance->_shouldTerminate) {
    if (xtimer_now_usec() - instance->_startTime >= RECEIVE_TIME_MS * 1000) {
      instance->stopReceiving();
      break;
    }

    xtimer_msleep(3);
    msg_receive(&msg);

    if (instance->_shouldTerminate) {
      break;
    }

    uart_t dev = (uart_t)msg.content.value;
    while (!ringbuffer_empty(&(_ctx[dev].rx_buf))) {
      uint8_t byte = ringbuffer_get_one(&(_ctx[dev].rx_buf));
      if (!packet_started) {
        if (byte == 0x54) {
          uint8_t next_byte;
          if (!ringbuffer_empty(&(_ctx[dev].rx_buf)) &&
              ringbuffer_peek(&_ctx[dev].rx_buf, (char *)&next_byte, 1) &&
              next_byte == 0x2C) {
            ringbuffer_get_one(&(_ctx[dev].rx_buf));
            packet_started = true;
            continue;
          }
        }
      } else {
        // Odbieranie danych po wykryciu sekwencji startowej
        data[data_index++] = byte;

        if (data_index >= 40) {
          if (byte == 0x54) {
            uint8_t next_byte;
            if (!ringbuffer_empty(&(_ctx[dev].rx_buf)) &&
                ringbuffer_peek(&_ctx[dev].rx_buf, (char *)&next_byte, 1) &&
                next_byte == 0x2C) {
              instance->processData(data, data_index - 1);
              data_index = 0;
              ringbuffer_get_one(&(_ctx[dev].rx_buf));
              continue;
            }
          }
        }

        if (data_index >= sizeof(data) / sizeof(data[0])) {
          packet_started = false;
          data_index = 0;
        }
      }
    }
  }
  printf("Terminating LidarUART data receiver thread\r\n");
  instance->_shouldTerminate = false;

  return NULL;
}

void LidarUART::processData(uint8_t *data, size_t len) {
  len++;
  len--;
  size_t new_len = 26;
  uint8_t temp_data[new_len];

  temp_data[0] = data[2];
  temp_data[1] = data[3];
  size_t temp_index = 2;
  for (size_t i = 4; i < 39; i++) {
    if (i % 3 == 0)
      continue;
    else
      temp_data[temp_index++] = data[i];
  }
  printf("D: ");
  printf("\r\n");
  uint8_t start = 0x54;
  uint8_t end = 0x2C;
  _bluetoothUART->send(&start, 1);
  _bluetoothUART->send(&end, 1);
  for (size_t i = 0; i < temp_index; i++) {
    _bluetoothUART->send(temp_data[i]);
  }
  //_bluetoothUART->send(temp_data, temp_index);
}

void LidarUART::startReceiving() {
  printf("Attempting to Start lidar processing thread\r\n");
  _shouldTerminate = false;
  if (_dataReceiverPid == KERNEL_PID_UNDEF) {
    _dataReceiverPid = thread_create(
        _dataReceiverThread, sizeof(_dataReceiverThread), DATARECEIVER_PRIO, 0,
        dataReceiveThread, this, "LidarReceiver");
  }
}

void LidarUART::stopReceiving() {
  printf("Attempting to Stop lidar processing thread\r\n");
  if (_dataReceiverPid != KERNEL_PID_UNDEF) {
    _shouldTerminate = true;
    msg_t msg;
    msg.content.value = (uint32_t)_dev;
    msg_send(&msg, _dataReceiverPid);

    // Zaczekaj na zakończenie wątku
    xtimer_msleep(50);
    _dataReceiverPid = KERNEL_PID_UNDEF;
    uint8_t end_sequence[4] = {0xFF, 0xFF, 0x00, 0xFF};
    _bluetoothUART->send(end_sequence, 4);
  }
}
