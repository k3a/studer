#define SERIAL_OK 0
#define SERIAL_ERR 1
#define SERIAL_NOT_ENOUGH 2
typedef unsigned serial_ret_t;

// initialize the serial port
int serial_init(const char* port_path, int speed, int parity);

// write to serial port size bytes from ptr
void serial_write(const void* ptr, unsigned size);

// read size bytes from serial into ptr buffer
serial_ret_t serial_read(void* ptr, unsigned size);

// read and skip bytes until your receive a byte b. Skip this byte as well and return. 
void serial_skip(unsigned char b);