// example serial IO implementation
// parts taken from https://stackoverflow.com/a/6947758/314437

#include "serial.h"

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define error_message(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

int serial_fd = 0;

static int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        //tty.c_cflag &= ~CRTSCTS; // disable RTS/CTS (hardware) flow control

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr", errno);
                return -1;
        }

        return 0;
}

static void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes", errno);
}

int serial_init(const char* port_path, int speed, int parity) {
  int ret;

  serial_fd = open(port_path, O_RDWR | O_NOCTTY | O_SYNC);
  if (serial_fd < 0) {
    error_message ("error %d opening %s: %s", errno, port_path, strerror(errno));
    return serial_fd;
  }

  // set speed and parity
  if ((ret = set_interface_attribs(serial_fd, parity, 0)) < 0) { 
    return ret;
  }

  //set_blocking (fd, 0); // set non-blocking

  return 0;
}

// write to serial port size bytes from ptr
void serial_write(const void* ptr, unsigned size) {
  int ret;

  if ((ret = write(serial_fd, ptr, size)) < 0) {
    error_message ("error writing to serial port: %s", strerror(errno));
  }
}

// read size bytes from serial into ptr buffer
serial_ret_t serial_read(void* ptr, unsigned size) {
  int ret = read(serial_fd, ptr, size);

  if (ret < 0) {
    return SERIAL_ERR;
  }

  if (ret < size) {
    return SERIAL_NOT_ENOUGH;
  }

  return SERIAL_OK;
}

// read and skip bytes until your receive a byte b. Skip this byte as well and return. 
void serial_skip(unsigned char b) {
  unsigned char readByte = 123;
  serial_ret_t ret;

  while ( (ret = serial_read(&readByte, 1) ) == SERIAL_OK ) {
    if (readByte == b) {
      break; // done reading
    }
  }
}