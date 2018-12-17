# Studer Solar Equipment Protcol Implementation

This code implements a serial protocol used in Xcom-232i from Studer Innotec. 
When seen as textual data, Studer protocol looks like AA3650001000E0762A2110BDB00100000CCF2.

The code was created according to their protocol documentation https://www.studer-innotec.com/media/document/0/technical-specification-xtender-serial-protocol-v1.6.30.pdf and was used in a working project based on ESP8266.

Sharing it for use by others, released under MIT license.

#### Usage:
```c
// input ac voltage command
int objID = OI_IN_AC_VOLT;
studer_send(DEST_XTM(0), SVC_READ_PROPERTY, OT_SYSTEM_STATE, objID, PI_VALUE);

// response in float with 4 bytes
float float_val;
if (studer_recv_check(&float_val, 4, objID)) {
  printf("Input AC voltage: %.0f V\n", float_val);
}
```

#### Serial IO:
You need to provide serial port functions.
An example implementation for Linux/BSD is provided.

```c
#define SERIAL_OK 0
typedef unsigned serial_ret_t;

// write to serial port size bytes from ptr
// returns number of bytes read or negative number indicating an error
int serial_write(const void* ptr, unsigned size);

// read size bytes from serial into ptr buffer
serial_ret_t serial_read(void* ptr, unsigned size);

// read and skip bytes until your receive a byte b. Skip this byte as well and return. 
void serial_skip(unsigned char b);
```

#### Notes

It was taken from a working project but cleaned by hand. If you find a problem or you have a suggestion, please open a pull request. Thanks!

There is now also an official scom library as part of Xcom-232i the tech spac zip which can be downloaded from https://www.studer-innotec.com/en/downloads/variotrack-series/. It may be a better fit for your project.
