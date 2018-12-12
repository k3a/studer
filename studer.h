//
//  Studer Protocol
//
//  Created by K3A (www.k3a.me) on 09.05.2015.
//  Released under MIT
//

#ifndef __iot__studer__
#define __iot__studer__

#include <stdbool.h>
#include <stdint.h>

// destinations
#define DEST_XTM(idx)       (101+idx)   // XTH/XTM inverter
#define DEST_MPPT(idx)      (301+idx)   // MPPT solar controller
#define DEST_MS             401         // Xcom MS - communication bridge for MPPT Tristar
#define DEST_232(idx)       (501+idx)   // Xcom-232i
#define DEST_BSP            601         // MSP battery controler
#define DEST_GATEWAY        1           // alias for the gateway that the DTE uses to communicate (the Xcom-232i to which you speak with RS-232)

// services
#define SVC_READ_PROPERTY   0x01
#define SVC_WRITE_PROPERTY  0x02

/// System state object
#define OT_SYSTEM_STATE     0x01
/// qsp parameters change
#define OT_PARAM_OBJECTS    0x02
/// Message
#define OT_MESSAGE          0x03    // set object_id to the index of desired message, returns struct, for details see 4.6

// inverter object IDs (accordign to 1.5.16)
#define OI_BATT_VOLTAGE     3000    // V, float
#define OI_BATT_TEMP        3001    // *C, float
#define OI_BATT_CHARGE_CURR 3005    // A, float
#define OI_BATT_VOLT_RIPPLE 3006    // V, float
#define OI_BATT_CYCLE_PHASE 3010    //# enum: 0-invalid, 1-bulk, 2-absorb, 3-equalise, 4-floating, 5-r.float., 6-per.abs., 7-missing, 8-forming
#define OI_IN_AC_VOLT       3011    // V, float
#define OI_IN_AC_CURR       3012    // A, float
#define OI_IN_AC_POWER      3013    //# kVA, float
#define OI_IN_LIMIT         3017    // A, float - limit value
#define OI_IN_LIMIT_REACHED 3018    // enum 0/1
#define OI_BOOST_ACTIVE     3019    // enum 0/1
#define OI_STATE_TRANSF_RLY 3020    // enum: 0-opened, 1-closed
#define OI_OUT_AC_VOLT      3021    // V, float
#define OI_OUT_AC_CURR      3022    // A, float
#define OI_OUT_AC_POWER     3023    //# kVA, float
#define OI_OPERATING_STATE  3028    // enum: 0-invalid, 1-inverter, 2-charger, 3-boost, 5-injection
#define OI_OUTPUT_RLY       3030    // enum: 0-opened, 1-closed
#define OI_AUX1_RLY         3031    // enum: 0-opened, 1-closed
#define OI_AUX2_RLY         3032    // enum: 0-opened, 1-closed
#define OI_NUM_OVERLOADS    3045    // float - number of overloads
#define OI_NUM_OVERTEMPS    3046    // float - number of overtemperatures
#define OI_NUM_BAT_OVERLOAD 3047    // float - number of battery overloads
#define OI_SYSTEM_STATE     3049    // enum: 0-off, 1-on
#define OI_NUM_BAT_ELEMENTS 3050    // float - number of battery elements
#define OI_SEARCH_MODE_STAT 3051    // enum: 0-off, 1-on
#define OI_AUX1_MODE        3054    // enum: 0-invalid, 1-A(automatic), 2-I(inv.automatic), 3-M(manual?), 4-M, 5-G(generator)
#define OI_AUX2_MODE        3055    // enum: 0-invalid, 1-A, 2-I, 3-M, 4-M, 5-G
#define OI_LOCKING_FLAGS    3056    // float bits: 0-forbidden inverter, 1-forbidden charger, 2-forbidden boost, 3-forbidden transfer, 4-forbidden injection, 8-mode integral, 9 f. multi, 10-mult independants, 11-standy slave allowed
#define OI_GROUND_RLY       3074    // enum: 0-opened, 1-closed
#define OI_NEUTRAL_XFER_RLY 3075    // enum: 0-opened, 1-closed
#define OI_DISCH_PREV_DAY   3076    // kWh, float
#define OI_DISCH_CURR_DAY   3078    // kWh, float
#define OI_INENERG_PREV_DAY 3080    // kWh, float
#define OI_INENERG_CURR_DAY 3081    //# kWh, float
#define OI_OENERG_PREV_DAY  3082    // kWh, float
#define OI_OENERG_CURR_DAY  3083    //# kWh, float
#define OI_IN_AC_FREQ       3084    // Hz, float
#define OI_OUT_AC_FREQ      3085    // Hz, float
#define OI_REM_ENTRY_STATE  3086    // enum: 0-RM EN 0, 1-RM EN 1
#define OI_OUT_ACTIVE_POWER 3087    // W, float - active power
#define OI_IN_ACTIVE_POWER  3088    // W, float
#define OI_DEFINED_PHASE    3089    // float 1-L1, 2-L2, 4-L3
#define OI_BAT_VOLT_MIN_MIN 3090    // V, float - minute min
#define OI_BAT_VOLT_MIN_MAX 3091    // V, float
#define OI_BAT_VOLT_MIN_AVG 3092    // V, float
#define OI_BAT_CHRC_MIN_MIN 3093    // A, float - charge current, minute min
#define OI_BAT_CHRC_MIN_MAX 3094    // A, float
#define OI_BAT_CHRC_MIN_AVG 3095    // A, float
#define OI_OUT_PWR_MIN_MIN  3096    // kVA, float - minute min
#define OI_OUT_PWR_MIN_MAX  3097    // kVA, float
#define OI_OUT_PWR_MIN_AVG  3098    // kVA, float
#define OI_OUT_APWR_MIN_MIN 3099    // kW, float - active power - minute min
#define OI_OUT_APWR_MIN_MAX 3100    // kW, float
#define OI_OUT_APWR_MIN_AVG 3101    // kW, float
#define OI_OUT_FREQ_MIN_MIN 3108    // Hz, float - output freq. - minute min
#define OI_OUT_FREQ_MIN_MAX 3109    // Hz, float
#define OI_OUT_FREQ_MIN_AVG 3110    // Hz, float
#define OI_IN_VOLT_MIN_MIN  3111    // V, float - minute min
#define OI_IN_VOLT_MIN_MAX  3112    // V, float
#define OI_IN_VOLT_MIN_AVG  3113    // V, float
#define OI_IN_CUR_MIN_MIN   3114    // A, float - input current - minute min
#define OI_IN_CUR_MIN_MAX   3115    // A, float
#define OI_IN_CUR_MIN_AVG   3116    // A, float
#define OI_IN_APWR_MIN_MIN  3117    // kW, float - active input power - minute min
#define OI_IN_APWR_MIN_MAX  3118    // kW, float
#define OI_IN_APWR_MIN_AVG  3119    // kW, float
#define OI_IN_FREQ_MIN_MIN  3120    // Hz, float - input freq. - minute min
#define OI_IN_FREQ_MIN_MAX  3121    // Hz, float
#define OI_IN_FREQ_MIN_AVG  3122    // Hz, float
#define OI_ID_TYPE          3124    // float - 1-XTH, 256-XTM, 512-XTS
#define OI_ID_POWER         3125    // VA, float
#define OI_ID_UOUT          3126    // V, float
#define OI_ID_BAT_VOLT      3127    // V, float
#define OI_ID_IOUT_NORM     3128    // A, float
#define OI_ID_HW            3129    // float
#define OI_ID_SOFT_MSB      3130
#define OI_ID_SOFT_LSB      3131
#define OI_ID_HW_PWR        3132
#define OI_PARAM_NUMBER     3133   
#define OI_USER_NUMBER      3134
#define OI_ID_SID           3135
/*
#define OI_OUT_ACTIVE_POWER 3136    // kW, float
#define OI_IN_ACTIVE_POWER  3137    // kW, float
#define OI_IN_POWER         3138    // kVA, float
#define OI_OUT_POWER        3139    // kVA, float
*/
#define OI_SYSTEM_SM        3142    // float

// BSP object IDs
#define OI_BSP_BATT_VOLTAGE 7000    //# V, float
#define OI_BSP_BATT_CURR    7001    //# A, float
#define OI_BSP_BATT_CHARGE  7002    //# %, float
#define OI_BSP_POWER        7003    //# W, float - power taken from battery
#define OI_BSP_REMAIN_AUTON 7004    //# minutes, float
#define OI_BSP_REL_CAPACITY 7006    //#? %, float
#define OI_BSP_CHARG_TODAY  7007    // Ah, float
#define OI_BSP_DISCH_TODAY  7008    // Ah, float
#define OI_BSP_CHARG_YESTER 7009    // Ah, float
#define OI_BSP_DISCH_YESTER 7010    // Ah, float
#define OI_BSP_CHARG_TOTAL  7011    // kAh, float
#define OI_BSP_DISCH_TOTAL  7012    // kAh, float
#define OI_BSP_TOTAL_TIME   7013    // days, float
#define OI_BSP_CUST_CHARG   7017    // Ah, float
#define OI_BSP_CUST_DISCH   7018    // Ah, float
#define OI_BSP_CUST_DURAT   7019    // hours, float
#define OI_BSP_TEMP         7029    //# *C, float
#define OI_BSP_BVOL_MIN_AVG 7030    // V, float - battery voltage - minute avg
#define OI_BSP_BCUR_MIN_AVG 7031    // A, float - battery current
#define OI_BSP_BCHG_MIN_AVG 7032    //# %, float - battery charge
#define OI_BSP_BTEM_MIN_AVG 7033    // *C, float - battery temperature
#define OI_BSP_ID_TYPE      7034    // float - 1-XTH, 256-XTM, 512-XTS
#define OI_BSP_ID_BAT_VOLT  7035    // V, float
#define OI_BSP_ID_HW        7036    // float
#define OI_BSP_ID_SOFT_MSB  7037
#define OI_BSP_ID_SOFT_LSB  7038
#define OI_BSP_PARAM_NUMBER 7039
#define OI_BSP_USER_NUMBER  7040
#define OI_BSP_ID_SID       7041


// MPPT (VarioTrack) objects
#define OI_MPPT_BATT_VOLT   11000   // V, float
#define OI_MPPT_BATT_CURR   11001   // A, float
#define OI_MPPT_PV_VOLT     11002   // V, float
#define OI_MPPT_PV_POWER    11004   //# kW, float
#define OI_MPPT_BATT_TEMP   11005   // *C, float
#define OI_MPPT_AH_CUR_DAY  11006   // Ah, float
#define OI_MPPT_KWH_CUR_DAY 11007   //# kWh, float
#define OI_MPPT_ENERG_RESCT 11008   // kWh, float
#define OI_MPPT_AH_PREV_DAY 11010   // Ah, float
#define OI_MPPT_KWN_PRE_DAY 11011   // kWh, float
#define OI_MPPT_VT_MODEL    11015   // model, enum: 0-Vt-80, 1-VT-65
#define OI_MPPT_OPER_MODE   11016   // enum: 0-night, 1-startUp, 2-, 3-charger, 4-, 5-security, 6-OFF, 7-, 8-charge, 9-chargeV, 10-chargeI, 11-chargeT
#define OI_MPPT_MAX_PVV_CD  11017   // V, float  - max pv volt curr day
#define OI_MPPT_MAX_CUR_CD  11018   // A, float - battery current
#define OI_MPPT_MAX_POW_CD  11019   // kW, float - max power for curr day
#define OI_MPPT_MAX_BV_CD   11029   // V, float - max batt voltage for curr day
#define OI_MPPT_MIN_BV_CD   11021   // V, float
#define OI_MPPT_NUM_IRR_CD  11025   // hours, float - num irraditation hours for the current day
#define OI_MPPT_NUM_IRR_PD  11026   // hours, float
#define OI_MPPT_ERROR_TYPE  11034   // enum: 0-no error, 1-BatOverV, 2-Earth, 3-NoBatt, 4-OverTemp, 5-BatOverV, 6-PvOverV, 7-others, 8-11--
#define OI_MPPT_DAYS_EQUAL  11037   // float, days until next equalization
#define OI_MPPT_BAT_CYCLE   11038   //# cycle phase, enum: 0-bulk, 1-absorb., 2-equalize, 3-floating, 4-, 5-, 6-r.float., 7-per.abs.
#define OI_MPPT_BV_MIN_AVG  11039   // V, float - battery voltage minute avg
#define OI_MPPT_BC_MIN_AVG  11040   // A, float - battery current min avg
#define OI_MPPT_PV_MIN_AVG  11041   // V, float - PV voltage
#define OI_MPPT_PP_MIN_AVG  11043   // kW, float - PV power
#define OI_MPPT_BT_MIN_AVG  11044   // *C, float - battery temperature
#define OI_MPPT_ID_TYPE     11047   // float - VT65 and VT80 = 9079d (0x2601)
#define OI_MPPT_ID_BAT_VOLT 11048   // V, float
#define OI_MPPT_ID_HW       11049   // float
#define OI_MPPT_ID_SOFT_MSB 11050
#define OI_MPPT_ID_SOFT_LSB 11051
#define OI_MPPT_ID_SID      11052
#define OI_MPPT_AUX1_RLY    11061   // enum: 0-opened, 1-closed
#define OI_MPPT_AUX2_RLY    11062   // enum: 0-opened, 1-closed
#define OI_MPPT_AUX1_MODE   11063   // enum: 0-invalid, 1-A(automatic), 2-I(inv.automatic), 3-M(manual?), 4-M, 5-G(generator)
#define OI_MPPT_AUX2_MODE   11064   // enum: 0-invalid, 1-A(automatic), 2-I(inv.automatic), 3-M(manual?), 4-M, 5-G(generator)
#define OI_MPPT_SYNC_STATE  11066   // enum: 0-3-, 4-XTslave, 5-VTslave, 6-7-, 8-VTmaster, 9-autonom
#define OI_MPPT_VT_STATE    11069   // ? not documented in 1.5.16

// property IDs
#define PI_VALUE            0x01    // state object value
#define PI_VALUE_QSP        0x05    // the value that can be entered on the remote control in level qsp or installer (one of the QSP_...)
#define PI_MIN_QSP          0x06    // minimum possible value
#define PI_MAX_QSP          0x07    // maximum
#define PI_LEVEL_QSP        0x08    // accessibility level of this parameter, enum 0x00-VIEW_ONLY, 0x10-BASIC, 0x20-EXPERT, 0x30-INSTALLER, 0x40-QSP

// QSP enum
#define QSP_VIEW_ONLY       0x00
#define QSP_BASIC           0x10
#define QSP_EXPERT          0x20
#define QSP_INSTALLER       0x30
#define QSP_QSP             0x40

// send data to Studer itnerface
void studer_send_data(uint16_t dest, uint8_t service_id, uint16_t object_type, uint32_t object_id, uint16_t property_id,
                      const void* data, unsigned len);

// send a command to Studer interface
void studer_send(uint16_t dest, uint8_t service_id, uint16_t object_type, uint32_t object_id, uint16_t property_id);

// receive a response from Studer interface
bool studer_recv(void* mem, uint16_t len);

#endif /*__iot__studer__*/
