#include <stdbool.h>

typedef enum {
    PARITY_NONE = 0,
    PARITY_EVEN = 1,
    PARITY_ODD = (1 << 1),
} serial_parity_t;

// initialize the serial port
int serial_init(const char *port_path, int speed, serial_parity_t parity, int stop_bits);

// write to serial port size bytes from ptr
int serial_write(const void *ptr, unsigned size);

// read size bytes from serial into ptr buffer
int serial_read(void *ptr, unsigned size);