# Studer Solar Equipment Protcol Implementation

This code implements a serial protocol used in Xcom-232i from Studer Innotec. 
When seen as textual data, Studer protocol looks like AA3650001000E0762A2110BDB00100000CCF2.

The code was created according to their protocol documentation http://www.studer-innotec.com/media/document/0/xcom_232i_user_manual_en.pdf and was used in a working project.

Sharing it for use by others. Released under MIT license.

#### Usage:
```c
// send the request for AC voltage
studer_send(DEST_XTM(0), SVC_READ_PROPERTY, OT_SYSTEM_STATE, OI_IN_AC_VOLT, PI_VALUE);

// receive it as a float value
float in_ac_volt;
if (studer_recv_check(&in_ac_volt, sizeof(in_ac_volt), it->object_id)) {
	printf("Input AC Voltage: %f\n", in_ac_volt);
}
```

#### Serial IO:
You need to provide serial port functions.

```c
#define SERIAL_OK 0
typedef unsigned serial_ret_t;

// write to serial port size bytes from ptr
void serial_write(const void* ptr, unsigned size);
// read size bytes from serial into ptr buffer
serial_ret_t serial_read(void* ptr, unsigned size);
// read and skip bytes until your receive a byte b. Skip this byte as well and return. 
void serial_skip(unsigned char b);

```

#### Notes

It was taken from a working project but cleaned by hand. If you find a problem or you have a suggestion, please open a pull request. Thanks.

