#include "scomlib_extra.h"

const char *scomx_err2str(scom_error_t err)
{
    switch (err) {
    case SCOM_ERROR_NO_ERROR:
        return "no error";
    case SCOM_ERROR_INVALID_FRAME:
        return "invalid frame";
    case SCOM_ERROR_DEVICE_NOT_FOUND:
        return "device not found";
    case SCOM_ERROR_RESPONSE_TIMEOUT:
        return "error response timeout";
    case SCOM_ERROR_SERVICE_NOT_SUPPORTED:
        return "service not supported";
    case SCOM_ERROR_INVALID_SERVICE_ARGUMENT:
        return "invalid service argument";
    case SCOM_ERROR_GATEWAY_BUSY:
        return "gateway busy";
    case SCOM_ERROR_TYPE_NOT_SUPPORTED:
        return "type not supported";
    case SCOM_ERROR_OBJECT_ID_NOT_FOUND:
        return "object id not found";
    case SCOM_ERROR_PROPERTY_NOT_SUPPORTED:
        return "property not supported";
    case SCOM_ERROR_INVALID_DATA_LENGTH:
        return "invalid data length";
    case SCOM_ERROR_PROPERTY_IS_READ_ONLY:
        return "property is read-only";
    case SCOM_ERROR_INVALID_DATA:
        return "invalid data";
    case SCOM_ERROR_DATA_TOO_SMALL:
        return "data too small";
    case SCOM_ERROR_DATA_TOO_BIG:
        return "data too big";
    case SCOM_ERROR_WRITE_PROPERTY_FAILED:
        return "write property failed";
    case SCOM_ERROR_READ_PROPERTY_FAILED:
        return "read property failed";
    case SCOM_ERROR_ACCESS_DENIED:
        return "access denied";
    case SCOM_ERROR_OBJECT_NOT_SUPPORTED:
        return "object not supported";
    case SCOM_ERROR_MULTICAST_READ_NOT_SUPPORTED:
        return "multicast read not supported";
    case SCOM_ERROR_INVALID_SHELL_ARG:
        return "invalid shell arg";
    case SCOM_ERROR_STACK_PORT_NOT_FOUND:
        return "stack port not found";
    case SCOM_ERROR_STACK_PORT_INIT_FAILED:
        return "stack port init failed";
    case SCOM_ERROR_STACK_PORT_WRITE_FAILED:
        return "stack port write failed";
    case SCOM_ERROR_STACK_PORT_READ_FAILED:
        return "stack port read failed";
    case SCOM_ERROR_STACK_BUFFER_TOO_SMALL:
        return "stack buffer too small";
    case SCOM_ERROR_STACK_PROPERTY_HEADER_DOESNT_MATCH:
        return "stack properety header doesn't match";
    default:
        return "unknown error";
    }
}