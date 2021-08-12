#ifndef SCOM_EXTRA_H
#define SCOM_EXTRA_H

#include "../scomlib/scom_data_link.h"
#include "../scomlib/scom_property.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
# How does scom work

There are two services only:
1. read property (0x01) - to read a property value
2. write property (0x02) - to write a new value to the property

There are multiple destinations (devices) to send data to, see the `SCOM_DEST_*`
The gateway (our device) has destination 0x1.

There are four object types:
1. User (sytem) info (0x1) with a single property (0x1) containing the current **READ-ONLY** state
   of various `object_id`s (specified in user manual of individual devices).
2. Parameters (0x2) with various **ADJUSTABLE** `object_id`s
   (described in the user manuals of individual devices or serial protocol appendix PDF)
   accessible via value_qsp (0x5) or unsaved_value_qsp (0xD) property_id (see the section `4.5.1`)
3. Message object (0x3) with `property_id` of 0x0 and message indexed from 0 using `object_id`
4. Datalog (0x0101) with object_id and property_id combinations described in section `4.8.1`

Status parameters which are read-only are read using 0x1 object type
and are called "User information ..." in the manual.

Adjustable parameters can be read or written usin 0x2 object type
and are called "Adjustment of ..." in the manual.

Example setup: XTM4000-48 inverter, VT-80 VarioTrack, BSP battery controller and RCC-02 console.
*/

// older name for the SCOM_USER_INFO_OBJECT_TYPE
// #define SCOM_SYSTEM_STATE_OBJECT_TYPE = (SCOM_USER_INFO_OBJECT_TYPE)

// 4.6 Message objects (firmware >= 1.5.0)
#define SCOM_MESSAGE_OBJECT_TYPE ((scom_object_type_t)0x3)

#define SCOM_DATALOG_TRANSFER_OBJECT_TYPE ((scom_object_type_t)0x0101)

// TYPES

typedef struct {
    /** \brief error that occurred during encoding */
    scom_error_t error;

    /** \brief pointer with the begining of the encoded data which needs to be send out; only valid
     * when no error is set */
    char *data;

    /** \brief length of the encoded data; only valid when no error is set */
    size_t length;
} scomx_enc_result_t;

typedef struct {
    /** \brief error that occurred during decoding */
    scom_error_t error;

    /** \brief number of additional bytes which needs to be read and passed to scomx_decode_frame */
    size_t length_to_read;
} scomx_header_dec_result_t;

typedef struct {
    /** \brief error that occurred during encoding */
    scom_error_t error;

    /** \brief source address of the sender */
    uint32_t src_addr;

    /** \brief service_id of the property decoded from the frame; only valid when no error is set */
    uint8_t service_id;

    /** \brief object_type of the property decoded from the frame; only valid when no error is set
     */
    uint16_t object_type;

    /** \brief object_id of the property decoded from the frame; only valid when no error is set */
    uint32_t object_id;

    /** \brief property_id of the property decoded from the framee; only valid when no error is set
     */
    uint16_t property_id;

    /** \brief pointer with the begining of the decoded property data; only valid when no error is
     * set */
    char *data;

    /** \brief length of the decoded property data; only valid when no error is set */
    size_t length;
} scomx_dec_result_t;

// DESTINATIONS

typedef uint32_t scomx_dest_t;

// XTH/XTM inverter
#define SCOMX_DEST_XTM(idx) ((scomx_dest_t)(101 + idx))
// MPPT solar controller
#define SCOMX_DEST_MPPT(idx) ((scomx_dest_t)(301 + idx))
// Xcom MS - communication bridge for MPPT Tristar
#define SCOMX_DEST_MS ((scomx_dest_t)(401))
// Xcom-232i
#define SCOMX_DEST_232(idx) ((scomx_dest_t)(501 + idx))
// MSP battery controler
#define SCOMX_DEST_BSP ((scomx_dest_t)(601))
#define SCOMX_DEST_GATEWAY ((scomx_dest_t)(1))

// USER INFO OBJECT_IDs

// NOTE: These objects are nicely documented in the "Technical specification - Xtender serial
// protocol appendix" PDF. That file is inside "Communication protocols Xcom-232i" zip file
// downloadable from "Software and Updates" sub-section of "Download center" at
// https://www.studer-innotec.com/

/** \brief Read-only user info objects */
typedef enum {
    // Xtender (Inverter)
    // (from RCC-02/-03 Quickguide 4.2.1 and RCC-02/-03 User manual 4.7.0)
    SCOMX_INFO_XTENDER_BATT_VOLTAGE = 3000,     // V, float
    SCOMX_INFO_XTENDER_BATT_TEMP = 3001,        // *C, float
    SCOMX_INFO_XTENDER_BATT_CHARGE_CURR = 3005, // A, float
    SCOMX_INFO_XTENDER_BATT_VOLT_RIPPLE = 3006, // V, float
    SCOMX_INFO_XTENDER_BATT_CYCLE_PHASE = 3010, //# enum: 0-invalid, 1-bulk, 2-absorb, 3-equalise, 4-floating, 5-r.float., 6-per.abs.,
                                                // 7-missing, 8-forming
    SCOMX_INFO_XTENDER_IN_AC_VOLT = 3011,       // V, float
    SCOMX_INFO_XTENDER_IN_AC_CURR = 3012,       // A, float
    SCOMX_INFO_XTENDER_IN_AC_POWER = 3013,      //# kVA, float
    SCOMX_INFO_XTENDER_IN_LIMIT = 3017,         // A, float - limit value
    SCOMX_INFO_XTENDER_IN_LIMIT_REACHED = 3018, // enum 0/1
    SCOMX_INFO_XTENDER_BOOST_ACTIVE = 3019,     // enum 0/1
    SCOMX_INFO_XTENDER_STATE_TRANSF_RLY = 3020, // enum: 0-opened, 1-closed
    SCOMX_INFO_XTENDER_OUT_AC_VOLT = 3021,      // V, float
    SCOMX_INFO_XTENDER_OUT_AC_CURR = 3022,      // A, float
    SCOMX_INFO_XTENDER_OUT_AC_POWER = 3023,     //# kVA, float
    SCOMX_INFO_XTENDER_OPERATING_STATE = 3028,  // enum: 0-invalid, 1-inverter, 2-charger, 3-boost, 5-injection
    SCOMX_INFO_XTENDER_OUTPUT_RLY = 3030,       // enum: 0-opened, 1-closed
    SCOMX_INFO_XTENDER_AUX1_RLY = 3031,         // enum: 0-opened, 1-closed
    SCOMX_INFO_XTENDER_AUX2_RLY = 3032,         // enum: 0-opened, 1-closed
    SCOMX_INFO_XTENDER_NUM_OVERLOADS = 3045,    // float - number of overloads
    SCOMX_INFO_XTENDER_NUM_OVERTEMPS = 3046,    // float - number of overtemperatures
    SCOMX_INFO_XTENDER_NUM_BAT_OVERLOAD = 3047, // float - number of battery overloads
    SCOMX_INFO_XTENDER_SYSTEM_STATE = 3049,     // enum: 0-off, 1-on
    SCOMX_INFO_XTENDER_NUM_BAT_ELEMENTS = 3050, // float - number of battery elements
    SCOMX_INFO_XTENDER_SEARCH_MODE_STAT = 3051, // enum: 0-off, 1-on
    SCOMX_INFO_XTENDER_AUX1_MODE = 3054,        // enum: 0-invalid, 1-A(automatic), 2-I(inv.automatic),
                                                // 3-M(manual?), 4-M, 5-G(generator)
    SCOMX_INFO_XTENDER_AUX2_MODE = 3055,        // enum: 0-invalid, 1-A, 2-I, 3-M, 4-M, 5-G
    SCOMX_INFO_XTENDER_LOCKING_FLAGS = 3056,    // float bits: 0-forbidden inverter, 1-forbidden charger, 2-forbidden boost,
                                                // 3-forbidden transfer, 4-forbidden injection, 8-mode integral, 9 f. multi, 10-mult
                                                // independants, 11-standy slave allowed
    SCOMX_INFO_XTENDER_GROUND_RLY = 3074,       // enum: 0-opened, 1-closed
    SCOMX_INFO_XTENDER_NEUTRAL_XFER_RLY = 3075, // enum: 0-opened, 1-closed
    SCOMX_INFO_XTENDER_DISCH_PREV_DAY = 3076,   // kWh, float
    SCOMX_INFO_XTENDER_DISCH_CURR_DAY = 3078,   // kWh, float
    SCOMX_INFO_XTENDER_INENERG_PREV_DAY = 3080, // kWh, float
    SCOMX_INFO_XTENDER_INENERG_CURR_DAY = 3081, //# kWh, float
    SCOMX_INFO_XTENDER_OENERG_PREV_DAY = 3082,  // kWh, float
    SCOMX_INFO_XTENDER_OENERG_CURR_DAY = 3083,  //# kWh, float
    SCOMX_INFO_XTENDER_IN_AC_FREQ = 3084,       // Hz, float
    SCOMX_INFO_XTENDER_OUT_AC_FREQ = 3085,      // Hz, float
    SCOMX_INFO_XTENDER_REM_ENTRY_STATE = 3086,  // enum: 0-RM EN 0, 1-RM EN 1
    SCOMX_INFO_XTENDER_OUT_ACTIVE_POWER = 3087, // W, float - active power
    SCOMX_INFO_XTENDER_IN_ACTIVE_POWER = 3088,  // W, float
    SCOMX_INFO_XTENDER_DEFINED_PHASE = 3089,    // float 1-L1, 2-L2, 4-L3
    SCOMX_INFO_XTENDER_BAT_VOLT_MIN_MIN = 3090, // V, float - minute min
    SCOMX_INFO_XTENDER_BAT_VOLT_MIN_MAX = 3091, // V, float
    SCOMX_INFO_XTENDER_BAT_VOLT_MIN_AVG = 3092, // V, float
    SCOMX_INFO_XTENDER_BAT_CHRC_MIN_MIN = 3093, // A, float - charge current, minute min
    SCOMX_INFO_XTENDER_BAT_CHRC_MIN_MAX = 3094, // A, float
    SCOMX_INFO_XTENDER_BAT_CHRC_MIN_AVG = 3095, // A, float
    SCOMX_INFO_XTENDER_OUT_PWR_MIN_MIN = 3096,  // kVA, float - minute min
    SCOMX_INFO_XTENDER_OUT_PWR_MIN_MAX = 3097,  // kVA, float
    SCOMX_INFO_XTENDER_OUT_PWR_MIN_AVG = 3098,  // kVA, float
    SCOMX_INFO_XTENDER_OUT_APWR_MIN_MIN = 3099, // kW, float - active power - minute min
    SCOMX_INFO_XTENDER_OUT_APWR_MIN_MAX = 3100, // kW, float
    SCOMX_INFO_XTENDER_OUT_APWR_MIN_AVG = 3101, // kW, float
    SCOMX_INFO_XTENDER_OUT_FREQ_MIN_MIN = 3108, // Hz, float - output freq. - minute min
    SCOMX_INFO_XTENDER_OUT_FREQ_MIN_MAX = 3109, // Hz, float
    SCOMX_INFO_XTENDER_OUT_FREQ_MIN_AVG = 3110, // Hz, float
    SCOMX_INFO_XTENDER_IN_VOLT_MIN_MIN = 3111,  // V, float - minute min
    SCOMX_INFO_XTENDER_IN_VOLT_MIN_MAX = 3112,  // V, float
    SCOMX_INFO_XTENDER_IN_VOLT_MIN_AVG = 3113,  // V, float
    SCOMX_INFO_XTENDER_IN_CUR_MIN_MIN = 3114,   // A, float - input current - minute min
    SCOMX_INFO_XTENDER_IN_CUR_MIN_MAX = 3115,   // A, float
    SCOMX_INFO_XTENDER_IN_CUR_MIN_AVG = 3116,   // A, float
    SCOMX_INFO_XTENDER_IN_APWR_MIN_MIN = 3117,  // kW, float - active input power - minute min
    SCOMX_INFO_XTENDER_IN_APWR_MIN_MAX = 3118,  // kW, float
    SCOMX_INFO_XTENDER_IN_APWR_MIN_AVG = 3119,  // kW, float
    SCOMX_INFO_XTENDER_IN_FREQ_MIN_MIN = 3120,  // Hz, float - input freq. - minute min
    SCOMX_INFO_XTENDER_IN_FREQ_MIN_MAX = 3121,  // Hz, float
    SCOMX_INFO_XTENDER_IN_FREQ_MIN_AVG = 3122,  // Hz, float
    SCOMX_INFO_XTENDER_ID_TYPE = 3124,          // float - 1-XTH, 256-XTM, 512-XTS
    SCOMX_INFO_XTENDER_ID_POWER = 3125,         // VA, float
    SCOMX_INFO_XTENDER_ID_UOUT = 3126,          // V, float
    SCOMX_INFO_XTENDER_ID_BAT_VOLT = 3127,      // V, float
    SCOMX_INFO_XTENDER_ID_IOUT_NORM = 3128,     // A, float
    SCOMX_INFO_XTENDER_ID_HW = 3129,            // float
    SCOMX_INFO_XTENDER_ID_SOFT_MSB = 3130,      //
    SCOMX_INFO_XTENDER_ID_SOFT_LSB = 3131,      //
    SCOMX_INFO_XTENDER_ID_HW_PWR = 3132,        //
    SCOMX_INFO_XTENDER_PARAM_NUMBER = 3133,     //
    SCOMX_INFO_XTENDER_USER_NUMBER = 3134,      //
    SCOMX_INFO_XTENDER_ID_SID = 3135,           //
    /*
    SCOMX_INFO_XTENDER_OUT_ACTIVE_POWER = 3136, // kW, float
    SCOMX_INFO_XTENDER_IN_ACTIVE_POWER = 3137,  // kW, float
    */
    SCOMX_INFO_XTENDER_IN_POWER = 3138,  // kVA, float
    SCOMX_INFO_XTENDER_OUT_POWER = 3139, // kVA, float
    SCOMX_INFO_XTENDER_SYSTEM_SM = 3142, // float

    // BSP Baterry Controller
    // (Quickguide RCC-02/-03 6.1.0)
    SCOMX_INFO_BSP_BATT_VOLTAGE = 7000, //# V, float
    SCOMX_INFO_BSP_BATT_CURR = 7001,    //# A, float
    SCOMX_INFO_BSP_BATT_CHARGE = 7002,  //# %, float
    SCOMX_INFO_BSP_POWER = 7003,        //# W, float - power taken from battery
    SCOMX_INFO_BSP_REMAIN_AUTON = 7004, //# minutes, float
    SCOMX_INFO_BSP_REL_CAPACITY = 7006, //#? %, float
    SCOMX_INFO_BSP_CHARG_TODAY = 7007,  // Ah, float
    SCOMX_INFO_BSP_DISCH_TODAY = 7008,  // Ah, float
    SCOMX_INFO_BSP_CHARG_YESTER = 7009, // Ah, float
    SCOMX_INFO_BSP_DISCH_YESTER = 7010, // Ah, float
    SCOMX_INFO_BSP_CHARG_TOTAL = 7011,  // kAh, float
    SCOMX_INFO_BSP_DISCH_TOTAL = 7012,  // kAh, float
    SCOMX_INFO_BSP_TOTAL_TIME = 7013,   // days, float
    SCOMX_INFO_BSP_CUST_CHARG = 7017,   // Ah, float
    SCOMX_INFO_BSP_CUST_DISCH = 7018,   // Ah, float
    SCOMX_INFO_BSP_CUST_DURAT = 7019,   // hours, float
    SCOMX_INFO_BSP_TEMP = 7029,         // *C, float - battery temperature
    SCOMX_INFO_BSP_BVOL_MIN_AVG = 7030, // V, float - battery voltage - minute avg
    SCOMX_INFO_BSP_BCUR_MIN_AVG = 7031, // A, float - battery current
    SCOMX_INFO_BSP_BCHG_MIN_AVG = 7032, //# %, float - battery charge
    SCOMX_INFO_BSP_BTEM_MIN_AVG = 7033, // *C, float - battery temperature
    SCOMX_INFO_BSP_ID_TYPE = 7034,      // float - 1-XTH, 256-XTM, 512-XTS
    SCOMX_INFO_BSP_ID_BAT_VOLT = 7035,  // V, float
    SCOMX_INFO_BSP_ID_HW = 7036,        // float
    SCOMX_INFO_BSP_ID_SOFT_MSB = 7037,  //
    SCOMX_INFO_BSP_ID_SOFT_LSB = 7038,  //
    SCOMX_INFO_BSP_PARAM_NUMBER = 7039, //
    SCOMX_INFO_BSP_USER_NUMBER = 7040,  //
    SCOMX_INFO_BSP_ID_SID = 7041,       //
    SCOMX_INFO_BSP_SMAN = 7047,         // SOC manufacturer
    SCOMX_INFO_BSP_LOCE = 7059,         // Local daily communication error counter (CAN)

    // VarioTrack (MPPT)
    // (Quickguide RCC-02/-03 6.1.0)
    SCOMX_INFO_VARIOTRACK_BATT_VOLT = 11000,   // V, float
    SCOMX_INFO_VARIOTRACK_BATT_CURR = 11001,   // A, float
    SCOMX_INFO_VARIOTRACK_PV_VOLT = 11002,     // V, float
    SCOMX_INFO_VARIOTRACK_PV_POWER = 11004,    //# kW, float
    SCOMX_INFO_VARIOTRACK_BATT_TEMP = 11005,   // *C, float
    SCOMX_INFO_VARIOTRACK_AH_CUR_DAY = 11006,  // Ah, float
    SCOMX_INFO_VARIOTRACK_KWH_CUR_DAY = 11007, //# kWh, float
    SCOMX_INFO_VARIOTRACK_ENERG_RESCT = 11008, // kWh, float
    SCOMX_INFO_VARIOTRACK_AH_PREV_DAY = 11010, // Ah, float
    SCOMX_INFO_VARIOTRACK_KWN_PRE_DAY = 11011, // kWh, float
    SCOMX_INFO_VARIOTRACK_VT_MODEL = 11015,    // model, enum: 0-Vt-80, 1-VT-65
    SCOMX_INFO_VARIOTRACK_OPER_MODE = 11016,   // enum: 0-night, 1-startUp, 2-, 3-charger, 4-, 5-security, 6-OFF, 7-, 8-charge,
                                               // 9-chargeV, 10-chargeI, 11-chargeT
    SCOMX_INFO_VARIOTRACK_MAX_PVV_CD = 11017,  // V, float  - max pv volt curr day
    SCOMX_INFO_VARIOTRACK_MAX_CUR_CD = 11018,  // A, float - battery current
    SCOMX_INFO_VARIOTRACK_MAX_POW_CD = 11019,  // kW, float - max power for curr day
    SCOMX_INFO_VARIOTRACK_MAX_BV_CD = 11029,   // V, float - max batt voltage for curr day
    SCOMX_INFO_VARIOTRACK_MIN_BV_CD = 11021,   // V, float
    SCOMX_INFO_VARIOTRACK_NUM_IRR_CD = 11025,  // hours, float - num irraditation hours for the current day
    SCOMX_INFO_VARIOTRACK_NUM_IRR_PD = 11026,  // hours, float
    SCOMX_INFO_VARIOTRACK_ERROR_TYPE = 11034,  // enum: 0-no error, 1-BatOverV, 2-Earth, 3-NoBatt,
                                               // 4-OverTemp, 5-BatOverV, 6-PvOverV, 7-others, 8-11--
    SCOMX_INFO_VARIOTRACK_DAYS_EQUAL = 11037,  // float, days until next equalization
    SCOMX_INFO_VARIOTRACK_BAT_CYCLE = 11038,   //# cycle phase, enum: 0-bulk, 1-absorb., 2-equalize,
                                               // 3-floating, 4-, 5-, 6-r.float., 7-per.abs.
    SCOMX_INFO_VARIOTRACK_BV_MIN_AVG = 11039,  // V, float - battery voltage minute avg
    SCOMX_INFO_VARIOTRACK_BC_MIN_AVG = 11040,  // A, float - battery current min avg
    SCOMX_INFO_VARIOTRACK_PV_MIN_AVG = 11041,  // V, float - PV voltage
    SCOMX_INFO_VARIOTRACK_PP_MIN_AVG = 11043,  // kW, float - PV power
    SCOMX_INFO_VARIOTRACK_BT_MIN_AVG = 11044,  // *C, float - battery temperature
    SCOMX_INFO_VARIOTRACK_ID_TYPE = 11047,     // float - VT65 and VT80 = 9079,d (0x2601)
    SCOMX_INFO_VARIOTRACK_ID_BAT_VOLT = 11048, // V, float
    SCOMX_INFO_VARIOTRACK_ID_HW = 11049,       // float
    SCOMX_INFO_VARIOTRACK_ID_SOFT_MSB = 11050,
    SCOMX_INFO_VARIOTRACK_ID_SOFT_LSB = 11051,
    SCOMX_INFO_VARIOTRACK_ID_SID = 11052,
    SCOMX_INFO_VARIOTRACK_AUX1_RLY = 11061,   // enum: 0-opened, 1-closed
    SCOMX_INFO_VARIOTRACK_AUX2_RLY = 11062,   // enum: 0-opened, 1-closed
    SCOMX_INFO_VARIOTRACK_AUX1_MODE = 11063,  // enum: 0-invalid, 1-A(automatic), 2-I(inv.automatic),
                                              // 3-M(manual?), 4-M, 5-G(generator)
    SCOMX_INFO_VARIOTRACK_AUX2_MODE = 11064,  // enum: 0-invalid, 1-A(automatic), 2-I(inv.automatic),
                                              // 3-M(manual?), 4-M, 5-G(generator)
    SCOMX_INFO_VARIOTRACK_SYNC_STATE = 11066, // enum: 0-3-, 4-XTslave, 5-VTslave, 6-7-, 8-VTmaster, 9-autonom
    SCOMX_INFO_VARIOTRACK_VT_STATE = 11069,   // state of VarioTrack (Off On)
    SCOMX_INFO_VARIOTRACK_LOCER = 11076,      // Local daily communication error counter (CAN)
    SCOMX_INFO_VARIOTRACK_RME = 11082,        // Remote entry state (RM EN 0, RM EN 1)

    // TriStar (MPPT)
    // TODO: not all objects listed here
    SCOMX_INFO_TRISTAR_TODO = 9000,
} scomx_user_info_object_t;

/** \brief Adjustable parameter objects */
typedef enum {
    // Remote Controller (RC)
    // TODO: not all objects listed here
    SCOMX_PARAM_RC_DATE = 5002, // unix timestamp of the current date/time

    // Xtender (Inverter)
    SCOMX_PARAM_XTENDER_BAT_CHARGER_ALLOWED = 1125,             // bool - Charger allowed
    SCOMX_PARAM_XTENDER_TRANSFER_RELAY_ALLOWED = 1128,          // bool - enable transfer relay
    SCOMX_PARAM_XTENDER_BAT_CYCLE_FORCE_NEW = 1142,             // int - Force a new cycle
    SCOMX_PARAM_XTENDER_BAT_EQUAL_FORCE = 1162,                 // int - Force equalization
    SCOMX_PARAM_XTENDER_SYS_BAT_PRIO = 1296,                    // bool - batteries priority as energy source (Not in parallel)
    SCOMX_PARAM_XTENDER_SYS_BAT_PRIO_VOLT = 1297,               // V, float - battery priority voltag
    SCOMX_PARAM_XTENDER_BAT_FLOAT_FORCE = 1467,                 // int - Force phase of floating
    SCOMX_PARAM_XTENDER_SYSTEM_REMOTE_ACTIVATED_BY_AUX1 = 1578, // bool - Activated by AUX1 state

    // BSP (Battery Controller)
    // TODO: not all objects listed here
    SCOMX_PARAM_BSP_TODO = 6000,

    // VarioTrack (MPPT)
    // TODO: not all objects listed here
    SCOMX_PARAM_VARIOTRACK_BAT_CYCLE_FORCE_NEW = 10029, // int - Force a new cycle
    SCOMX_PARAM_VARIOTRACK_BAT_ABSOR_FORCE = 10010,     // int - Force absorption phase
    SCOMX_PARAM_VARIOTRACK_BAT_EQUAL_FORCE = 10018,     // int - Force equalization
    SCOMX_PARAM_VARIOTRACK_BAT_FLOAT_FORCE = 10006,     // int - Force floating
} scomx_parameter_object_t;

typedef enum {
    // "user info" object "Value" property
    SCOMX_PROP_USER_INFO_VALUE = 0x1,
    // "parameter" object "value_qsp" that can be entered on the remote control in level qsp or
    // installer (one of the QSP_...)
    SCOMX_PROP_PARAMETER_VALUE_QSP = 0x5,
    // "parameter" object "min_qsp" - Minimum that canbeentered on the remote control in level qsp
    // or installer
    SCOMX_PROP_PARAMETER_MIN_QSP = 0x6,
    // "parameter" object "max_qsp" - Maximum that can be entered on the remote control in level qsp
    // or installe
    SCOMX_PROP_PARAMETER_MAX_QSP = 0x7,
    // "parameter" object "level_qsp" - accessibility level of this parameter modifiable in level
    // qsp or install enum 0x00-VIEW_ONLY, 0x10-BASIC, 0x20-EXPERT, 0x30-INSTALLER, 0x40-QSP
    SCOMX_PROP_PARAMETER_LEVEL_QSP = 0x8,
    // "parameter" object "unsaved_value_qsp" - the value that can be entered on the remote control
    // in level qsp or installer, but without saving the value in internal flas
    SCOMX_PROP_PARAMETER_UNSAVED_VALUE_QSP = 0xD,
} scomx_property_t;

// FUNCTIONS

// Returns static string describing the error
const char *scomx_err2str(scom_error_t err);

// FUNCTIONS - REQUEST ENCODING

// Encodes a generic property read request
scomx_enc_result_t scomx_encode_read_property(uint32_t dst_addr, scom_object_type_t object_type, uint32_t object_id, uint16_t property_id);
// Encodes a request to read "value" property of an "user info-type" (0x1) object
scomx_enc_result_t scomx_encode_read_user_info_value(scomx_dest_t dst_addr, scomx_user_info_object_t object_id);
// Encodes a request to read "value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_read_parameter_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id);
// Encodes a request to read "value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_read_parameter_unsaved_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id);
// Encodes a request to read "min_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_read_parameter_min(scomx_dest_t dst_addr, scomx_parameter_object_t object_id);
// Encodes a request to read "max_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_read_parameter_max(scomx_dest_t dst_addr, scomx_parameter_object_t object_id);
// Encodes a request to read "level_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_read_parameter_level(scomx_dest_t dst_addr, scomx_parameter_object_t object_id);

// Encodes a generic property write request
scomx_enc_result_t scomx_encode_write_property(uint32_t dst_addr, scom_object_type_t object_type, uint32_t object_id, uint16_t property_id, const char *const data,
                                               size_t data_len);

// Encodes writing into the "value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, const char *const data, size_t data_len);
// Encodes writing into the "value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_value_u32(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint32_t val);
// Encodes writing into the "value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_value_u16(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint16_t val);
// Encodes writing into the "value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_value_float(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, float val);

// Encodes writing into the "unsaved_value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_unsaved_value(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, const char *const data, size_t data_len);
// Encodes writing into the "unsaved_value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_unsaved_value_u32(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint32_t val);
// Encodes writing into the "unsaved_value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_unsaved_value_u16(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, uint16_t val);
// Encodes writing into the "unsaved_value_qsp" property of a "parameter-type" (0x2) object
scomx_enc_result_t scomx_encode_write_parameter_unsaved_value_float(scomx_dest_t dst_addr, scomx_parameter_object_t object_id, float val);

// FUNCTIONS - RESPONSE ENCODING

// Decode frame header from data read from the port. Read data must be exactly
// SCOM_FRAME_HEADER_SIZE bytes long.
scomx_header_dec_result_t scomx_decode_frame_header(const char *const data, size_t data_len);
// Decode the rest of the frame (after the header)
scomx_dec_result_t scomx_decode_frame(const char *const data, size_t data_len);

// FUNCTIONS - RESPONSE RESULT DATA TYPE DECODING

// Reads native-endian uint32_t or uint16_t value from the response if the response is valid and
// long enough
uint32_t scomx_result_int(scomx_dec_result_t res);
// Reads native float value from the response if the response is valid and long enough. May not work
// on all platforms.
float scomx_result_float(scomx_dec_result_t res);

#ifdef __cplusplus
}
#endif

#endif
