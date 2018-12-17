#include <termios.h>
#include <stdio.h>

#include "serial.h"
#include "studer.h"

int main() {
  printf("Studer example comm\n");

  // change serial port params according to your needs
  // speed, parity and stop bit is documented in technical specification for Xcom-232i
  if (serial_init("/dev/ttyUSB2", B38400/*B115200*/, PARITY_EVEN, 1) != 0) {
    return 1;
  }

  // input ac voltage command 
  int objID = OI_IN_AC_VOLT;
  studer_send(DEST_XTM(0), SVC_READ_PROPERTY, OT_SYSTEM_STATE, objID, PI_VALUE);
 
  // response in float with 4 bytes
  float float_val; 
  if (studer_recv_check(&float_val, 4, objID)) { 
    printf("Input AC voltage: %.0f V\n", float_val);
  } else {
    printf("ERROR: problem receiving value\n");
  }

  return 0;
}
