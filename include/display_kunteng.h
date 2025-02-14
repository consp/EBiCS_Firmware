/*
 * display_kunteng.h
 *
 *  Created on: 25.09.2019
 *      Author: Admin
 */

#ifndef DISPLAY_KUNTENG_H_
#define DISPLAY_KUNTENG_H_

#include "config.h"

void kunteng_init();
void display_update(MotorState_t* MS_U);
void check_message(MotorState_t* MS_D, MotorParams_t* MP_D);

typedef struct _lcd_configuration_variables
{
  uint8_t ui8_assist_level;
  uint8_t ui8_light;
  uint8_t ui8_motor_characteristic;
  uint8_t ui8_wheel_size;
  uint8_t ui8_max_speed;
  uint8_t ui8_power_assist_control_mode;
  uint8_t ui8_controller_max_current;
  uint8_t ui8_p1;
  uint8_t ui8_p2;
  uint8_t ui8_p3;
  uint8_t ui8_p4;
  uint8_t ui8_p5;
  uint8_t ui8_c1;
  uint8_t ui8_c2;
  uint8_t ui8_c4;
  uint8_t ui8_c5;
  uint8_t ui8_c12;
  uint8_t ui8_c13;
  uint8_t ui8_c14;
} struc_lcd_configuration_variables;


// Considering the follow voltage values for each li-ion battery cell
// State of charge 		| voltage
#define LI_ION_CELL_VOLTS_MAX 4.20
#define LI_ION_CELL_VOLTS_100 4.10
#define LI_ION_CELL_VOLTS_80 3.90 // 4.02
#define LI_ION_CELL_VOLTS_60 3.70 // 3.87
#define LI_ION_CELL_VOLTS_40 3.50 // 3.80
#define LI_ION_CELL_VOLTS_20 3.30 // 3.73
#define LI_ION_CELL_VOLTS_0 3.10 // 3.27
#define LI_ION_CELL_VOLTS_MIN 3.00

#define BATTERY_PACK_VOLTS_100	(LI_ION_CELL_VOLTS_100 * BATTERY_LI_ION_CELLS_NUMBER) * 1000
#define BATTERY_PACK_VOLTS_80 	(LI_ION_CELL_VOLTS_80 * BATTERY_LI_ION_CELLS_NUMBER) * 1000
#define BATTERY_PACK_VOLTS_60	(LI_ION_CELL_VOLTS_60 * BATTERY_LI_ION_CELLS_NUMBER) * 1000
#define BATTERY_PACK_VOLTS_40	(LI_ION_CELL_VOLTS_40 * BATTERY_LI_ION_CELLS_NUMBER) * 1000
#define BATTERY_PACK_VOLTS_20	(LI_ION_CELL_VOLTS_20 * BATTERY_LI_ION_CELLS_NUMBER) * 1000
#define BATTERY_PACK_VOLTS_0	(LI_ION_CELL_VOLTS_0 * BATTERY_LI_ION_CELLS_NUMBER) * 1000

#define ADC_BATTERY_VOLTAGE_K 73 // 0.272 << 8
#define BATTERY_LI_ION_CELLS_NUMBER 7

#define COMMUNICATIONS_BATTERY_VOLTAGE	(BATTERY_LI_ION_CELLS_NUMBER * 3.45) // example: 7S battery, should be = 24

#pragma pack (push, 1)
/*
 * note, settings C3, C6, C7, C8, C9, C10, C11 and L4 are all display settings and not transmitted to the controller
 */
typedef struct LCD8_display_data_t {
	union  {
		uint8_t raw[13];
		struct {
			uint8_t 	p5; // B0           // 0-64                                     // Power mode, if ALLOW_DYNAMIC_CURRENT is set, sets the max current
			uint8_t 	assist_level : 3;   // 0-5, 6 = cruise
			uint8_t 	unknown : 4;        
			uint8_t 	lights : 1;         // 0-1
			uint8_t 	wheel_size_msb : 3;
			uint8_t 	max_speed_lsb : 5;  // B2
			uint8_t 	p1; // B3           // 0-255                                    // Number of magnets * motor gearing
			uint8_t 	p2 : 3;             // 0-7                                      // Wheel speed pulse setting, 0 = external, 1-6 is internal, DOES NOT WORK, BUILD SETTING 
			uint8_t 	p3 : 1;             // 0-1                                      // Assist mode, NOT AVAILABLE
			uint8_t 	p4 : 1;             // 0-1                                      // Legal cheat mode, note: having this enabled might be illegal, NOT AVAILABLE
			uint8_t 	max_speed_msb : 1;
			uint8_t 	wheel_size_lsb : 1; // note, wheel size is NOT 5 bit but 4;
            uint8_t     l2 : 1;// B4        // 0-1                                      // Pretty much MSB of P1 allowing up to 512 to be selected. NOT AVAILABLE
			uint8_t 	crc; // B5
			uint8_t 	c2 : 3;             // 0-1, sometimes 0-6 or 0-7                // Reverse rotation, only available if ALLOW_DYNAMIC_REVERSE is enabled, 0 = REVERSE = 1, 1 = REVERSE = -1
			uint8_t		c1 : 3;             // 0-7                                      // PAS characteristics, NOT AVAILABLE
			uint8_t 	unkown2 : 2; // B6
			uint8_t 	c5 : 4;             // 0-10                                     // Current adjustment, NOT AVAILABLE
			uint8_t 	unknown4 : 1;
			uint8_t 	c14 : 2;            // 1-3                                      // Assist Level adjustment, NOT AVAILABLE
			uint8_t 	unknown3 : 1;// B7
			uint8_t 	c12 : 5;            // 0-7                                      // Controller minimum voltage setting, NOT AVAILABLE
			uint8_t 	c4 : 3; // B8       // 0-4, 4 sets c4_percentage                // Legal cheat mode, note: having this enabled might be illegal, NOT AVAILABLE
			uint8_t 	B9; // B9
            uint8_t     l3 : 1;             // 0-1                                      // Dual motor setting, NOT AVAILABLE
			uint8_t 	unknown6 : 1;
			uint8_t 	c13 : 3;            // 0-5                                      // Regen settings, NOT AVAILABLE
			uint8_t 	c15 : 2;            // 0-2 (4-6)                                
			uint8_t 	unknown5 : 1; // B10
			uint8_t 	c4_percentage : 6;  // 0-40 (20-60)                             // This option is presented when C4 == 4
			uint8_t 	l1 : 2; // B11      // 0-3                                      // Undervoltage setting, NOT AVAILABLE
			uint8_t 	B12;
		};
	};
	uint8_t max_speed;
	uint8_t wheel_size;
} LCD8_display_data;

typedef struct LCD8_controller_data_t {
	union {
		uint8_t raw[12];
		struct {
			uint8_t B0; // always 0x41
			uint8_t charging_status : 2;
			uint8_t bars : 3;
			uint8_t unknown1 : 3; // B1
			uint8_t nominal_voltage : 6;
			uint8_t reverse : 1; // shows reverse text
			uint8_t unknown2 : 1; // B2
			uint16_t rotation; // B3+B4 // note: endianess
			uint8_t error; // B5
			uint8_t crc; // B6
			uint8_t mode_normal : 1;
			uint8_t mode_throttle : 1;
			uint8_t mode_cruise_icon : 1; // might not work
			uint8_t mode_cruise : 1; // might set both icon and text
			uint8_t mode_assist : 1;
			uint8_t mode_brake : 1;
			uint8_t unknown4 : 2; // B7
			uint8_t amps; // B8
			uint8_t motor_temperature; // B9
			uint8_t B10;
			uint8_t B11;
			uint8_t B12;
		};
	};
} LCD8_controller_data;
#pragma pack(pop)

#endif /* DISPLAY_KUNTENG_H_ */
