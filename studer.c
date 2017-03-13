//
//  Studer Protocol
//
//  Created by K3A (www.k3a.me) on 09.05.2015.
//  Released under MIT
//

#include "utils.h"
#include "serial.h"
#include "studer.h"

// Studer Innotec VT 80A - voltage regulator
// Studer Innotec XTM 4000-48 - hybrid inverter
// RCC-02 - remote control console
// BTS-01 - battery temperature sensor
// RCM-10 - remote  control module (relay?)
// BSP 500 - battery status processor (with a shunt of 500 A)
// MS - Communication bridge to solar voltage regulators

typedef struct
{
    //uint8_t start;
    uint8_t flags;
    uint32_t src;
    uint32_t dst;
    uint16_t len;
} __attribute__((packed)) StuderHeader; // 11 bytes (without start byte)
#define STUDER_HEADER_SZ 11

typedef struct
{
    uint8_t flags;
    uint8_t service_id;
    // common service_data for READ_PROPERTY/WRITE_PROPERTY follows:
    uint16_t object_type;
    uint32_t object_id;
    uint16_t property_id;
} __attribute__((packed)) StuderBody; // 10 bytes
#define STUDER_BODY_SZ 10

static uint16_t studer_sum(const void* data, uint16_t len)
{
    unsigned A = 0xFF;
    unsigned B = 0;
    uint16_t checksum;
    
    for (unsigned i=0; i<len; i++)
    {
        A = (A + ((uint8_t*)data)[i]) % 0x100;
        B = (B + A) % 0x100;
    }
    
    uint8_t* ch = (uint8_t*)&checksum;
    ch[0] = A;
    ch[1] = B;
    
    return checksum;
}

void studer_send_data(uint16_t dest, uint8_t service_id, uint16_t object_type, uint32_t object_id, uint16_t property_id,
                      const void* data, unsigned len)
{
    uint16_t sum;
    StuderHeader head;
    
    //head.start = 0xAA;
    uint8_t start = 0xAA;
    serial_write(&start, 1);
    
    head.flags = 0;
    head.src = 1;
    head.dst = dest; //101;
    head.len = STUDER_BODY_SZ+len;
    
    // head and head sum
    serial_write(&head, STUDER_HEADER_SZ);
    //el_hexdump(&head, STUDER_HEADER_SZ);
    sum = studer_sum(&head, STUDER_HEADER_SZ);
    serial_write(&sum, sizeof(sum));
    //el_hexdump(&sum, sizeof(sum));
    
    // body
    uint8_t* body = (uint8_t*)malloc(STUDER_BODY_SZ+len);
    if (!body) {
        printf("Unable to allocate memory for the Studer body\n");
        return;
    }
    
    StuderBody b;
    b.flags = 0;
    b.service_id = service_id;
    b.object_type = object_type;
    b.object_id = object_id;
    b.property_id = property_id;
    memcpy(body, &b, STUDER_BODY_SZ);
    if (data) memcpy(body+STUDER_BODY_SZ, data, len);
    serial_write(body, STUDER_BODY_SZ+len);
    //el_hexdump(body, STUDER_BODY_SZ+len);
    
    // body sum
    sum = studer_sum(body, STUDER_BODY_SZ+len);
    serial_write(&sum, sizeof(sum));
    //el_hexdump(&sum, sizeof(sum));
    
    free(body);
}

void studer_send(uint16_t dest, uint8_t service_id, uint16_t object_type, uint32_t object_id, uint16_t property_id)
{
    studer_send_data(dest, service_id, object_type, object_id, property_id, NULL, 0);
}

static bool studer_recv_with_body(void* mem, uint16_t len, StuderBody* ret_body)
{
    uint16_t sum;
    uint16_t computedSum;
    unsigned ret = 0;
    unsigned retries = 0;
    uint8_t* recv;
    StuderHeader h;
    StuderBody b;
    
AGAIN:
    
    // skip input until 0xAA is received
    serial_skip(0xAA); // normally should skip only this first 0xAA byte
    
    recv = (uint8_t*)malloc( STUDER_HEADER_SZ+sizeof(sum) );
    if (!recv) {
        printf("Unable to allocate mem for recv stured header\n");
        return false;
    }
    ret = serial_read(recv, STUDER_HEADER_SZ+sizeof(sum));
    if (ret != SERIAL_OK)
    {
        printf("Unable to read Studer header. Error %d\n", ret);
        free(recv);
        return false;
    }
    
    // head and head sum

    //AGAIN: //TODO: fix position, set timeout again
    memcpy(&h, recv, STUDER_HEADER_SZ);
    memcpy(&sum, recv+STUDER_HEADER_SZ, sizeof(sum));
    computedSum = studer_sum(recv, STUDER_HEADER_SZ);
    
    free(recv);
    
    if (computedSum != sum)
    {
        printf("Wrong header checksum: 0x%X (computed) != 0x%X (received)\n", computedSum, sum);
        if (mem) memset(mem, 0, len); // zero memory
        return false;
    }
    
    // body
    h.len -= STUDER_BODY_SZ; // get length of data after the body header
    
    //uint8_t recv2[] = {0x02, 0x01, 0x01, 0x00, 0xB8, 0x0B, 0x00, 0x00, 0x01, 0x00, 0x00, 0xA0, 0xBB, 0x41,/* <- 14 bytes of body */
    //    0x63, 0x57}; /* < sum of body */
    recv = (uint8_t*)malloc( STUDER_BODY_SZ+h.len+sizeof(sum) );
    if (!recv) {
        printf("Unable to allocate mem for recv stured body\n");
        return false;
    }
    ret = serial_read(recv, STUDER_BODY_SZ+h.len+sizeof(sum));
    if (ret != SERIAL_OK)
    {
        printf("Unable to read Studer body with data. Error %d\n", ret);
        free(recv);
        return false;
    }
    
    // check the body sum
    computedSum = studer_sum(recv, STUDER_BODY_SZ+h.len);
    memcpy(&sum, recv+STUDER_BODY_SZ+h.len, sizeof(sum));
    if (computedSum != sum)
    {
        printf("Wrong body checksum: 0x%X (computed) != 0x%X (received)\n", computedSum, sum);
        if (mem) emset(mem, 0, len); // zero memory
        free(recv);
        return false;
    }
    
    // fill the body struct
    memcpy(&b, recv, STUDER_BODY_SZ);
    if (ret_body) memcpy(ret_body, &b, STUDER_BODY_SZ);
    
    bool is_error = b.flags & 1;
    bool is_response = (b.flags >> 1) & 1;
    
    if (is_error)
    {
        uint16_t error_id = 0;
        if (h.len == 2)
            memcpy(&error_id, recv+STUDER_BODY_SZ, 2);
        printf("Received an error response 0x%X\n", error_id);
        free(recv);
        return false;
    }
    if (!is_response) // just in case..
    {
        printf("Skipping a request frame because waiting for a response frame\n");
        free(recv);
        
        if (retries++ < 3)
            goto AGAIN;
        else
            return false;
    }
    
    // copy body data
    if (mem && h.len>0)
    {
        if (h.len > len)
        {
            printf("Received data bigger (%u) than requested (%d). Truncating.\n", h.len, len);
            h.len = len;
        }
        memcpy(mem, recv+STUDER_BODY_SZ, h.len);
        
        // zero-terminate if possible
        char* cmem = (char*)mem;
        if (len > h.len)
            *(cmem+h.len) = 0;
    }
    
    free(recv);
    return true; // receive successful
}

bool studer_recv(void* mem, uint16_t len)
{
    return studer_recv_with_body(mem, len, NULL);
}

static bool studer_recv_check(void* mem, uint16_t len, uint32_t object_id)
{
    StuderBody body;
    bool ret = studer_recv_with_body(mem, len, &body);
    if (ret && body.object_id != object_id)
    {
        printf("Ignoring response for other object id %u, wanted %u\n", body.object_id, object_id);
        return false;
    }
    return ret;
}






