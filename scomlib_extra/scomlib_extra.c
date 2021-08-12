#include "scomlib_extra.h"
#include "../scomlib/scom_property.h"

#include <string.h>

// allocate memory on the heap
// normally only a single serial port is used so there is no need to use multiple buffers
static char g_buffer[256];
static scom_frame_t g_frame;
static scom_property_t g_property;

#define SCOM_SERVICE_HEADER_SIZE 2
#define SCOM_PROPERTY_HEADER_SIZE 8

#define SCOM_PROPERTY_HEADER_OFFSET (SCOM_FRAME_HEADER_SIZE + SCOM_SERVICE_HEADER_SIZE)
#define SCOM_PROPERTY_VALUE_OFFSET (SCOM_FRAME_HEADER_SIZE + SCOM_SERVICE_HEADER_SIZE + SCOM_PROPERTY_HEADER_SIZE)

/* copied from scom library: internal function to decode the values that identify a property */
static void scom_decode_property_header(scom_property_t* property)
{
    char* header =  property->frame->buffer + SCOM_PROPERTY_HEADER_OFFSET;

    property->object_type = (scom_object_type_t)scom_read_le16(&header[0]);
    property->object_id = scom_read_le32(&header[2]);
    property->property_id = scom_read_le16(&header[6]);
}

static void reset_frame() {
    // init frame
    scom_initialize_frame(&g_frame, g_buffer, sizeof(g_buffer));

    // clear the buffer just in case
    memset(g_frame.buffer, 0, g_frame.buffer_size);

    // init property
    scom_initialize_property(&g_property, &g_frame);
}

static scomx_enc_result_t encode_request_frame() {
    scomx_enc_result_t res;

    memset(&res, 0, sizeof(res));

    // check error of the previous scomx_encode_*_property call done on this frame
    if (g_frame.last_error != SCOM_ERROR_NO_ERROR) {
        res.error = g_frame.last_error;
        return res;
    }

    scom_encode_request_frame(&g_frame);

    res.error = g_frame.last_error;
    res.data = g_frame.buffer;
    res.length = scom_frame_length(&g_frame);

    return res;   
}

scomx_enc_result_t scomx_encode_read_property(uint32_t dst_addr, scom_object_type_t object_type, uint32_t object_id, uint16_t property_id) {
    reset_frame();

    g_frame.src_addr = 1; // our address
    g_frame.dst_addr = dst_addr;

    g_property.object_type = object_type;
    g_property.object_id = object_id;
    g_property.property_id = property_id;

    scom_encode_read_property(&g_property);

    return encode_request_frame(); 
}

scomx_enc_result_t scomx_encode_write_property(uint32_t dst_addr, scom_object_type_t object_type, uint32_t object_id, uint16_t property_id, const char* const data, size_t data_len) {
    reset_frame();

    g_frame.src_addr = 1; // our address
    g_frame.dst_addr = dst_addr;

    g_property.object_type = object_type;
    g_property.object_id = object_id;
    g_property.property_id = property_id;

    g_property.value_length = data_len;

    // ensure data fits into the buffer
    if (scom_frame_length(&g_frame) > g_frame.buffer_size) {
        scomx_enc_result_t res;
        res.error = SCOM_ERROR_STACK_BUFFER_TOO_SMALL;
        return res;
    }

    // scom requires continuous buffer so we must copy the data into the buffer
    g_property.value_length = data_len;
    memcpy(g_property.value_buffer, data, data_len);

    scom_encode_write_property(&g_property);

    return encode_request_frame(); 
}

scomx_enc_result_t scomx_encode_read_user_info_value(scomx_dest_t dst_addr, scomx_user_info_object_t object_id) {
    return scomx_encode_read_property(dst_addr, SCOM_USER_INFO_OBJECT_TYPE, object_id, SCOMX_PROP_USER_INFO_VALUE);
}

scomx_enc_result_t scomx_encode_read_parameter_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id) {
    return scomx_encode_read_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_VALUE_QSP);
}

scomx_enc_result_t scomx_encode_read_parameter_min(scomx_dest_t dst_addr, scomx_parameter_object_t object_id) {
    return scomx_encode_read_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_MIN_QSP);
}

scomx_enc_result_t scomx_encode_read_parameter_max(scomx_dest_t dst_addr, scomx_parameter_object_t object_id) {
    return scomx_encode_read_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_MAX_QSP);
}

scomx_enc_result_t scomx_encode_read_parameter_unsaved_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id) {
    return scomx_encode_read_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_UNSAVED_VALUE_QSP);
}

scomx_enc_result_t scomx_encode_read_parameter_level(scomx_dest_t dst_addr, scomx_parameter_object_t object_id) {
    return scomx_encode_read_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_LEVEL_QSP);
}

scomx_enc_result_t scomx_encode_write_parameter_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, const char* const data, size_t data_len) {
    return scomx_encode_write_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_VALUE_QSP, data, data_len);
}

scomx_enc_result_t scomx_encode_write_parameter_value_u32(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint32_t val) {
    char buf[4];
    scom_write_le32(buf, val);
    return scomx_encode_write_parameter_value(dst_addr, object_id, buf, sizeof(buf));
}

scomx_enc_result_t scomx_encode_write_parameter_value_u16(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint16_t val) {
    char buf[2];
    scom_write_le16(buf, val);
    return scomx_encode_write_parameter_value(dst_addr, object_id, buf, sizeof(buf));
}

scomx_enc_result_t scomx_encode_write_parameter_value_float(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, float val) {
    char buf[4];
    scom_write_le_float(buf, val);
    return scomx_encode_write_parameter_value(dst_addr, object_id, buf, sizeof(buf));
}

scomx_enc_result_t scomx_encode_write_parameter_unsaved_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, const char* const data, size_t data_len) {
    return scomx_encode_write_property(dst_addr, SCOM_PARAMETER_OBJECT_TYPE, object_id, SCOMX_PROP_PARAMETER_UNSAVED_VALUE_QSP, data, data_len);
}

scomx_enc_result_t scomx_encode_write_parameter_unsaved_value_u32(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint32_t val) {
    char buf[4];
    scom_write_le32(buf, val);
    return scomx_encode_write_parameter_unsaved_value(dst_addr, object_id, buf, sizeof(buf));
}

scomx_enc_result_t scomx_encode_write_parameter_unsaved_value_u16(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint16_t val) {
    char buf[2];
    scom_write_le16(buf, val);
    return scomx_encode_write_parameter_unsaved_value(dst_addr, object_id, buf, sizeof(buf));
}

scomx_enc_result_t scomx_encode_write_parameter_unsaved_value_float(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, float val) {
    char buf[4];
    scom_write_le_float(buf, val);
    return scomx_encode_write_parameter_unsaved_value(dst_addr, object_id, buf, sizeof(buf));
}

scomx_header_dec_result_t scomx_decode_frame_header(const char* const data, size_t data_len) {
    scomx_header_dec_result_t res;

    memset(&res, 0, sizeof(res));

    reset_frame();

    if(data_len != SCOM_FRAME_HEADER_SIZE) {
        // read data must be exactly SCOM_FRAME_HEADER_SIZE bytes long
        res.error = SCOM_ERROR_STACK_PORT_READ_FAILED;
        return res;
    }

    // copy header data
    memcpy(g_frame.buffer, data, data_len);

    // decode the header
    scom_decode_frame_header(&g_frame);
    if(g_frame.last_error != SCOM_ERROR_NO_ERROR) {
        res.error = g_frame.last_error;
        return res;
    }

    // request reading the data part
    res.length_to_read = scom_frame_length(&g_frame) - SCOM_FRAME_HEADER_SIZE;

    return res;
}

scomx_dec_result_t scomx_decode_frame(const char* const data, size_t data_len) {
    scomx_dec_result_t res;

    memset(&res, 0, sizeof(res));

    if(data_len != scom_frame_length(&g_frame) - SCOM_FRAME_HEADER_SIZE) {
        // read data must be of specific length
        res.error = SCOM_ERROR_STACK_PORT_READ_FAILED;
        return res;
    }

    // copy frame data into the buffer
    memcpy(&g_frame.buffer[SCOM_FRAME_HEADER_SIZE], data, data_len);

    // decode frame data
    scom_decode_frame_data(&g_frame);

    res.error = g_frame.last_error;

    res.src_addr = g_frame.src_addr;
    res.service_id = g_frame.service_id;

    // reuse the structure
    scom_initialize_property(&g_property, &g_frame);

    // decode the read or write property service
    ptrdiff_t length = (ptrdiff_t)g_property.frame->data_length - SCOM_SERVICE_HEADER_SIZE - SCOM_PROPERTY_HEADER_SIZE;

    if(!g_property.frame->service_flags.error
        && length >= 0
        && length <= (ptrdiff_t)g_property.value_buffer_size) {
            // property looks ok
            g_property.value_length = length;
            scom_decode_property_header(&g_property);
    } else if(g_property.frame->service_flags.error) {
        // decode application error
        if (length == 2) {
            res.error = (scom_error_t)scom_read_le16(g_property.value_buffer);
        } else {
            res.error = SCOM_ERROR_INVALID_FRAME;
        }
        return res;
    } else {
        // no application error but value over the property buffer size
        res.error = SCOM_ERROR_STACK_BUFFER_TOO_SMALL;
        return res;
    }

    // copy property data into the dec result
    res.object_type = g_property.object_type;
    res.object_id = g_property.object_id;
    res.property_id = g_property.property_id;

    res.data = g_property.value_buffer;
    res.length = g_property.value_length;

    return res;
}

uint32_t scomx_result_int(scomx_dec_result_t res) {
    if (res.error == SCOM_ERROR_NO_ERROR) {
        if (res.length >= 4) {
            return scom_read_le32(res.data);
        } else if (res.length >= 2) {
            return scom_read_le16(res.data);
        }
    }
    return 0;
}

float scomx_result_float(scomx_dec_result_t res) {
    if (res.error == SCOM_ERROR_NO_ERROR && res.length >= 4) {
        return scom_read_le_float(res.data);
    }
    return 0;
}