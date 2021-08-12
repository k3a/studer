#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "../scomlib_extra/scomlib_extra.h"
#include "serial.h"

static void hex_dump(const void* inmem, size_t len)
{
	const unsigned char* mem = (const unsigned char*)inmem;

    for(unsigned row=0; row<=len/16; row++)
    {
        printf("0x%04X:  ", row*16);
        
        // print hexa
        for (unsigned tb=row*16; tb<row*16+16; tb+=2)
        {
            if (tb==len)
            {
                for(unsigned dummy=tb; dummy<row*16+16; dummy+=2)
                    printf("     ");
                break;
            }
            else if (tb+1==len)
            {
                printf("%02X   ", mem[tb]);
                for(unsigned dummy=tb+2; dummy<row*16+16; dummy+=2)
                    printf("     ");
                break;
            }
            else
                printf("%02X%02X ", mem[tb], mem[tb+1] );
        }
        
        printf(" ");
        
        // print ascii
        for (unsigned a=row*16; a<row*16+16; a++)
        {
            if (a==len) break;
            
            unsigned char chr = mem[a];
            if (chr<32 || chr>126)
                chr = '.';
            
            printf("%c", chr );
        }
        
        printf("\n");
    }
}

int test() {
	scomx_enc_result_t encresult;
	scomx_header_dec_result_t dechdr;
	scomx_dec_result_t decres;
	size_t bytecounter;
	char readbuf[128];
	float outval = 0.0;

	encresult = scomx_encode_read_user_info_value(SCOMX_DEST_XTM(0), SCOMX_INFO_XTENDER_OUT_AC_POWER);

	printf("WRITING FRAME:\n");
	hex_dump(encresult.data, encresult.length);
	bytecounter = serial_write(encresult.data, encresult.length);
	if (bytecounter != encresult.length) {
		printf("Wrote only %u bytes from %u\n", bytecounter, encresult.length);
		return 10;
	}

	printf("READING HEAD:\n");
	bytecounter = serial_read(readbuf, SCOM_FRAME_HEADER_SIZE);
	if (bytecounter != SCOM_FRAME_HEADER_SIZE) {
		printf("Read only %u bytes from %u (header)\n", bytecounter, SCOM_FRAME_HEADER_SIZE);
		return 1;
	}
	hex_dump(readbuf, SCOM_FRAME_HEADER_SIZE);

	dechdr = scomx_decode_frame_header(readbuf, SCOM_FRAME_HEADER_SIZE);
	if (dechdr.error != SCOM_ERROR_NO_ERROR) {
		printf("Error decoding frame header: %s\n", scomx_err2str(dechdr.error));
		return 2;
	}

	printf("READING BODY:\n");
	bytecounter = serial_read(readbuf, dechdr.length_to_read);
	if (bytecounter != dechdr.length_to_read) {
		printf("Read only %u bytes from %u (body)\n", bytecounter, dechdr.length_to_read);
		return 3;
	}
	hex_dump(readbuf, dechdr.length_to_read);

	decres = scomx_decode_frame(readbuf, dechdr.length_to_read);
	if (decres.error != SCOM_ERROR_NO_ERROR) {
		printf("Error decoding frame body: %s\n", scomx_err2str(decres.error));
		return 4;
	}

	outval = scomx_result_float(decres);

	printf("SRC ADDR: %u, SVC ID %u, OBJ TYPE %u, OBJ ID %u, PROP ID %u, VALUE %.3f\n", 
		decres.src_addr, decres.service_id, decres.object_type, decres.object_id, decres.property_id, outval);

	return 0;
}

int main(int argc, const char* argv[]) {
	const char* port = "/dev/ttyUSB0";

	if (argc > 1) {
		port = argv[1];
	}

	printf("Studer serial comm test on port %s\n", port);
	if (serial_init(port, B38400, PARITY_EVEN, 1) != 0) {
		return 1;
	}

	for (unsigned i=0; i<3; i++) {
		printf("=> attempt %u:\n", i);
		int r = test();
		if (r != 0) {
			printf("=== RET CODE %d\n", r);
		}
	}
}
