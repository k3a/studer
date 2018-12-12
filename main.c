#include <termios.h>
#include <stdio.h>

#include "serial.h"
#include "studer.h"

int main() {
  printf("Studer example comm\n");

  if (serial_init("/dev/ttyUSB2", B115200, 0) != 0) {
    return 1;
  }

  //in_ac_volt: DEST_XTM(0), OI_IN_AC_VOLT 
  studer_send(DEST_XTM(0), SVC_READ_PROPERTY, OT_SYSTEM_STATE, OI_IN_AC_VOLT, PI_VALUE);

  // response in float with 4 bytes
  float float_val; 
  if (studer_recv(&float_val, 4)) {
     printf("Input AC voltage: %.0f V\n", float_val);
  } else {
    printf("ERROR: problem receiving value\n");
  }

  return 0;
}