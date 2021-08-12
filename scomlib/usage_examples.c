/*
Copyright (c) 2014 Studer Innotec SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#if 0

/**
 * \file usage_examples.c examples of usage of the scom library
 */

#include <stdio.h>
#include <string.h>
#include <scom_property.h>

/* ---------------------------------------------------------------*/
/* these functions must be implemented on the targeted platform
 * to have a runable example.
 */

/**
 * \brief configure the serial port
 * \return zero in case of an error
 */
int initialize_serial_port();

/**
 * \brief empty the rx and tx buffer before a new exchange
 */
void clear_serial_port();

/**
 * \brief read in a blocking way on the serial port
 *
 * This function must implement the proper timeout mechanism.
 * * \return number of byte read
 */
size_t read_serial_port(char* buffer, size_t byte_count);

/**
 * \brief write in a blocking way on the serial port
 *
 * This function must implement the proper timeout mechanism.
 * \return number of byte written
 */
size_t write_serial_port(const char* buffer, size_t byte_count);

/**
 * \brief close the serial port even if initialize_serial_port() failed
 */
void close_serial_port();


/*  ----------------------examples function definitions ---------------------- */
scom_error_t exchange_frame(scom_frame_t* frame);
void read_xt1_uBat();


int main()
{
    if(initialize_serial_port() != 0) {
        read_xt1_uBat();
    }
    else {
        printf("error in the serial port initialisation\n");
    }

    close_serial_port();


    return 0;
}


/**
 * \brief example code to read the battery voltage on Xtender 1 and print it to the standard output
 */
void read_xt1_uBat()
{

    scom_frame_t frame;
    scom_property_t property;
    char buffer[1024];

    /* initialize the structures */
    scom_initialize_frame(&frame, buffer, sizeof(buffer));
    scom_initialize_property(&property, &frame);

    frame.src_addr = 1;     /* my address, could be anything */
    frame.dst_addr = 101;   /* the first inverter */

    property.object_type = SCOM_USER_INFO_OBJECT_TYPE;  /* read a user info */
    property.object_id = 3000;  /* the battery voltage */

    scom_encode_read_property(&property);

    if(frame.last_error != SCOM_ERROR_NO_ERROR) {
        printf("read property frame encoding failed with error %d\n", (int) frame.last_error);
        return;
    }

    /* do the exchange of frames */
    if(exchange_frame(&frame)!= SCOM_ERROR_NO_ERROR) {
        return;
    }

    /* reuse the structure to save space */
    scom_initialize_property(&property, &frame);

    /* decode the read property service part */
    scom_decode_read_property(&property);
    if(frame.last_error != SCOM_ERROR_NO_ERROR) {
        printf("read property decoding failed with error %d\n", (int) frame.last_error);
        return;
    }

    /* check the the size */
    if(property.value_length != 4) {
        printf("invalid property data response size\n");
        return;
    }

    /* print the result */
    printf("uBat = %.2f V\n", scom_read_le_float(property.value_buffer));
}


/**
 * \brief example code to exchange a frame and print possible error on standard output
 *
 * \param frame an initialized frame configured for a service
 * \return a possible error that occurred or SCOM_NO_ERROR
 */
scom_error_t exchange_frame(scom_frame_t* frame)
{
    size_t byte_count;

    clear_serial_port();

    scom_encode_request_frame(frame);

    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link frame encoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    /* send the request on the com port */

    byte_count = write_serial_port(frame->buffer, scom_frame_length(frame));
    if(byte_count != scom_frame_length(frame)) {
        printf("error when writing to the com port\n");
        return SCOM_ERROR_STACK_PORT_WRITE_FAILED;
    }

    /* reuse the structure to save space */
    scom_initialize_frame(frame, frame->buffer, frame->buffer_size);

    /* clear the buffer for debug purpose */
    memset(frame->buffer,0, frame->buffer_size);

    /* Read the fixed size header.
       A platform specific handling of a timeout mechanism should be
       provided in case of the possible lack of response */

    byte_count = read_serial_port(&frame->buffer[0], SCOM_FRAME_HEADER_SIZE);

    if(byte_count != SCOM_FRAME_HEADER_SIZE) {
        printf("error when reading the header from the com port\n");
        return SCOM_ERROR_STACK_PORT_READ_FAILED;
    }

    /* decode the header */
    scom_decode_frame_header(frame);
    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link header decoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    /* read the data part */

    byte_count = read_serial_port(&frame->buffer[SCOM_FRAME_HEADER_SIZE],  scom_frame_length(frame) - SCOM_FRAME_HEADER_SIZE);
    if(byte_count != (scom_frame_length(frame) - SCOM_FRAME_HEADER_SIZE)) {
        printf("error when reading the data from the com port\n");
        return SCOM_ERROR_STACK_PORT_READ_FAILED;
    }

    /* decode the data */
    scom_decode_frame_data(frame);
    if(frame->last_error != SCOM_ERROR_NO_ERROR) {
        printf("data link data decoding failed with error %d\n", (int) frame->last_error);
        return frame->last_error;
    }

    return SCOM_ERROR_NO_ERROR;

}

#endif