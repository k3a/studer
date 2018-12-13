#include <stdbool.h>

typedef enum {
  SERIAL_OK = 0,
  SERIAL_ERR = 1,
  SERIAL_NOT_ENOUGH = 2,
} serial_ret_t;

typedef enum {
  PARITY_NONE = 0,
  PARITY_EVEN = 1,
  PARITY_ODD = (1<<1), 
} serial_parity_t;


// initialize the serial port
int serial_init(const char* port_path, int speed, serial_parity_t parity, int stop_bits);

// write to serial port size bytes from ptr
int serial_write(const void* ptr, unsigned size);

// read size bytes from serial into ptr buffer
serial_ret_t serial_read(void* ptr, unsigned size);

// read and skip bytes until your receive a byte b. Skip this byte as well and return. 
void serial_skip(unsigned char b);