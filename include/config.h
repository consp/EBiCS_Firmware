/*
 * config.h
 *
 *  Automatically created by Lishui Parameter Configurator
 *  Author: stancecoke
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include "stdint.h"
#define DISPLAY_TYPE_EBiCS (1<<5)  
#define DISPLAY_TYPE_KINGMETER_618U (1<<4)                  // King-Meter 618U protocol (KM5s, EBS-LCD2, J-LCD, SW-LCD)
#define DISPLAY_TYPE_KINGMETER_901U (1<<8)                  // King-Meter 901U protocol (KM5s)
#define DISPLAY_TYPE_KINGMETER      (DISPLAY_TYPE_KINGMETER_618U|DISPLAY_TYPE_KINGMETER_901U)
#define DISPLAY_TYPE_BAFANG (1<<2)							// For 'Blaupunkt' Display of Prophete Entdecker
#define DISPLAY_TYPE_KUNTENG (1<<1)							// For ASCII-Output in Debug mode
#define DISPLAY_TYPE_DEBUG (1<<0)							// For ASCII-Output in Debug mode);
#define EXTERNAL 1
#define INTERNAL 0

#define TRIGGER_OFFSET_ADC 50
#define TRIGGER_DEFAULT 2020
#define _T 2028

// Calibration values
#define CAL_BAT_V 256
#define CAL_V 15LL<<8
// #define CAL_I 13LL<<8
// #define CAL_I 38LL<<8
#define CAL_I 106LL<<8

#define INDUCTANCE 6
#define RESISTANCE 40
#define FLUX_LINKAGE 1200
#define GAMMA 9

// battery levels
#define BATTERY_LEVEL_1 210000
#define BATTERY_LEVEL_2 230000
#define BATTERY_LEVEL_3 260000
#define BATTERY_LEVEL_4 280000
#define BATTERY_LEVEL_5 294000

// pid values
#define P_FACTOR_I_Q 700
#define I_FACTOR_I_Q 24
#define P_FACTOR_I_D 700
#define I_FACTOR_I_D 24

// #define P_FACTOR_PLL 7
// #define I_FACTOR_PLL 5
#define P_FACTOR_PLL 11
#define I_FACTOR_PLL 9

#define SPDSHFT 0

#define P_FACTOR_SPEED 100
#define I_FACTOR_SPEED 10

#define SPEEDFILTER 0
// limit speed without legal flag set, so you can set max with throttle
#define LIMIT_SPEED_WITHOUT_LEGAL_FLAG
#define SIXSTEPTHRESHOLD 12000
// #define SIXSTEPTHRESHOLD 4000
#define TS_COEF 60000
// pas
#define PAS_TIMEOUT 4000
#define RAMP_END 1600
#define FRAC_HIGH 30
#define FRAC_LOW 15
// throttle
#define THROTTLE_OFFSET 900
#define THROTTLE_MAX 3000
// push assist
#define PUSHASSIST_CURRENT 65

// wheel
#define WHEEL_CIRCUMFERENCE 2200
// gear ratio, magnets/2
#define GEAR_RATIO 10
// speed limit in kph
#define SPEEDLIMIT 30
#define PULSES_PER_REVOLUTION 1
// max phase current
#define PH_CURRENT_MAX 188 // ~ 20A
#define BATTERYCURRENT_MAX 12000
#define REGEN_CURRENT 46 // ~ 5A
#define REGEN_CURRENT_MAX 10000 // My BMS limit
// voltage, see cal
#define VOLTAGE_MIN 820
#define VOLTAGE_MAX 1160
//
// #define SPEC_ANGLE -715827882
// #define DISPLAY_TYPE DISPLAY_TYPE_DEBUG //ASCII Printout for debugging
// #define DISPLAY_TYPE DISPLAY_TYPE_KINGMETER_901U
// #define DISPLAY_TYPE DISPLAY_TYPE_KINGMETER_618U
#define DISPLAY_TYPE DISPLAY_TYPE_KUNTENG
// #define DISPLAY_TYPE DISPLAY_TYPE_BAFANG
#define SPEED_PLL 1 // not needed
#define SPEEDSOURCE INTERNAL
#define AUTODETECT 0
#define REVERSE 1
#define THROTTLE_OVERRIDE

/* LCD8 Settings */
#define ALLOW_DYNAMIC_CURRENT                // Uncomment to allow P5 to controll the max battery current
#define ALLOW_DYNAMIC_REVERSE                // Uncomment to allow C2 to change the direction of rotation
#define ALLOW_DYNAMIC_REGEN                  // Uncomment to allow C5 to set regen current in A
#define BRAKELIGHT_IS_BACKLIGHT              // Uncomment to use the brakelight signal/port for the backlight

#define THROTTLE_SWITCH_AT_STARTUP

#endif /* CONFIG_H_ */
