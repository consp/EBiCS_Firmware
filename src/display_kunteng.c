/*
 * display_kunteng.c
 *
 *  Created on: 25.09.2019
 *      Author: Admin
 */

#include "main.h"
#include "display_kunteng.h"
#include "stm32f1xx_hal.h"
#include "eeprom.h"
#define iabs(x) (((x) >= 0)?(x):-(x))
extern volatile uint32_t adcData[9];
static uint8_t ui8_tx_buffer[12];
extern int8_t i8_reverse_flag;
uint8_t ui8_j;
uint8_t ui8_crc;
uint8_t ui8_last_XOR;
uint16_t ui16_wheel_period_ms =4500;
uint32_t ui32_battery_volts= 36;
uint8_t ui8_battery_soc = 12;
uint8_t ui16_error;
static uint8_t ui8_rx_buffer[13];
uint8_t ui8_rx_initial_buffer[13];
uint8_t ui8_rx_buffer_counter = 0;
uint8_t ui8_byte_received;
uint8_t ui8_moving_indication = 0;
uint8_t ui8_UARTCounter = 0;
uint8_t ui8_msg_received=0;
int16_t i16_eeprom_temp=0;
uint8_t ui8_gear_ratio = GEAR_RATIO;

LCD8_display_data *displaydata = (LCD8_display_data *) ui8_rx_buffer;
LCD8_controller_data *controllerdata = (LCD8_controller_data *) ui8_tx_buffer;

volatile struc_lcd_configuration_variables lcd_configuration_variables;

extern UART_HandleTypeDef huart1;
void check_recent(void);

void kunteng_init()
{

    if (HAL_UART_Receive_DMA(&huart1, (uint8_t *)ui8_rx_buffer, 13) != HAL_OK)
     {
 	   Error_Handler();
     }

    EE_ReadVariable(EEPROM_KT_B0_B3, &i16_eeprom_temp);
    ui8_rx_buffer[0] = i16_eeprom_temp>>8;
    ui8_rx_initial_buffer[0] = i16_eeprom_temp>>8;
    ui8_rx_buffer[3] = i16_eeprom_temp &0xFF;
    ui8_rx_initial_buffer[3] = i16_eeprom_temp &0xFF;

    EE_ReadVariable(EEPROM_KT_B2_B4, &i16_eeprom_temp);
    ui8_rx_buffer[2] = i16_eeprom_temp>>8;
    ui8_rx_initial_buffer[2] = i16_eeprom_temp>>8;
    ui8_rx_buffer[4] = i16_eeprom_temp &0xFF;
    ui8_rx_initial_buffer[4] = i16_eeprom_temp &0xFF;

    EE_ReadVariable(EEPROM_KT_B6_B7, &i16_eeprom_temp);
    ui8_rx_buffer[6] = i16_eeprom_temp>>8;
    ui8_rx_initial_buffer[6] = i16_eeprom_temp>>8;
    ui8_rx_buffer[7] = i16_eeprom_temp &0xFF;
    ui8_rx_initial_buffer[7] = i16_eeprom_temp &0xFF;

    EE_ReadVariable(EEPROM_KT_B8_B9, &i16_eeprom_temp);
    ui8_rx_buffer[8] = i16_eeprom_temp>>8;
    ui8_rx_initial_buffer[8] = i16_eeprom_temp>>8;
    ui8_rx_buffer[9] = i16_eeprom_temp &0xFF;
    ui8_rx_initial_buffer[9] = i16_eeprom_temp &0xFF;

    EE_ReadVariable(EEPROM_KT_B1_B10, &i16_eeprom_temp);
    ui8_rx_buffer[1] = i16_eeprom_temp>>8;
    ui8_rx_initial_buffer[1] = i16_eeprom_temp>>8;
    ui8_rx_buffer[10] = i16_eeprom_temp &0xFF;
    ui8_rx_initial_buffer[10] = i16_eeprom_temp &0xFF;

    ui8_crc = 0;

    for (ui8_j = 0; ui8_j <= 12; ui8_j++)
    {
      if (ui8_j == 5) continue; // don't xor B5 (B7 in our case)
      ui8_crc ^= ui8_rx_buffer[ui8_j];
    }
    ui8_crc ^=10; //right XOR must be pasted here!!!
    ui8_rx_buffer [5]=ui8_crc;
    displaydata->B12=0x37;
    ui8_rx_buffer[12]=0x0E;

}

void display_update(MotorState_t* MS_U)
{

   // prepare moving indication info
  ui8_moving_indication = 0;
  //if (ebike_app_cruise_control_is_set ()) { ui8_moving_indication |= (1 << 3); }
  if (throttle_is_set ()) { ui8_moving_indication |= (1 << 1); }
  /* if (pas_is_set ()) { ui8_moving_indication |= (1 << 4); } */


  // calc battery pack state of charge (SOC)
  // voltage is 0.025v per mv on ADC for LW17xx
  ui32_battery_volts =  (MS_U->Voltage*25);
  if (ui32_battery_volts >= (BATTERY_PACK_VOLTS_100)) {
      controllerdata->charging_status = 2; // 2 = Charging indicator
      controllerdata->bars = 0;
  } else if (ui32_battery_volts > ((uint16_t) BATTERY_PACK_VOLTS_80)) { 
      controllerdata->charging_status = 2; // 0 = normal indicator
      controllerdata->bars = 4;
  } else if (ui32_battery_volts > ((uint16_t) BATTERY_PACK_VOLTS_60)) { 
      controllerdata->charging_status = 0; // 0 = normal indicator
      controllerdata->bars = 3;
  } // 3 bars
  else if (ui32_battery_volts > ((uint16_t) BATTERY_PACK_VOLTS_40)) { 
      controllerdata->charging_status = 0; // 0 = normal indicator
      controllerdata->bars = 2;
  } // 2 bars
  else if (ui32_battery_volts > ((uint16_t) BATTERY_PACK_VOLTS_20)) {
      controllerdata->charging_status = 0; // 0 = normal indicator
      controllerdata->bars = 1;
  } // 1 bar
  else if (ui32_battery_volts > ((uint16_t) BATTERY_PACK_VOLTS_0)) {
      controllerdata->charging_status = 0; // 0 = normal 
      controllerdata->bars = 0;
  } else { 
      controllerdata->charging_status = 1; // 1 = flashing
      controllerdata->bars = 0;
  }
#if SPEEDSOURCE == EXTERNAL
  ui16_wheel_period_ms = (MS_U->Speed*PULSES_PER_REVOLUTION)>>3; //for External speedsensor
#else
  ui16_wheel_period_ms= (MS_U->Speed*6*((uint16_t)ui8_gear_ratio))/500;
#endif
  ui8_tx_buffer [0] =  65;
  // B1: battery level
  // B2: 24V controller
  controllerdata->nominal_voltage = (uint8_t) COMMUNICATIONS_BATTERY_VOLTAGE;
  controllerdata->reverse = MS_U->direction == -1 ? 1 : 0;
  // B3: speed, wheel rotation period, ms; period(ms)=B3*256+B4;
  controllerdata->rotation = (ui16_wheel_period_ms >> 8) | (ui16_wheel_period_ms << 8);

  // B5: error info display
  controllerdata->error = ui16_error;
  // B6: CRC: xor B1,B2,B3,B4,B5,B7,B8,B9,B10,B11
  // 0 value so no effect on xor operation for now
  controllerdata->crc = 0;
  // B7: moving mode indication, bit
  //
  controllerdata->mode_brake = brake_is_set();
  controllerdata->mode_throttle = throttle_is_set();
  controllerdata->mode_assist = pas_is_set();

  // B8: 4x controller current
  // Vbat = 30V:
  // - B8 = 255, LCD shows 1912 watts
  // - B8 = 250, LCD shows 1875 watts
  // - B8 = 100, LCD shows 750 watts
  // each unit of B8 = 0.25A


  //ui8_tx_buffer [8] =  (uint8_t)(((ui16_BatteryCurrent-ui16_current_cal_b+1)<<2)/current_cal_a);
  /* ui8_tx_buffer [8] =  (uint8_t)(MS_U->Battery_Current*MS_U->Voltage*CAL_BAT_V/82010000);   //Kalibrierung nach Binatone, empririsch ermittelt. Strom und Spannung in Milli, 13W pro digit */
  controllerdata->amps = iabs(MS_U->Battery_Current) / 250; // LCD8 takes it's own voltage for reference.
  // B9: motor temperature
  controllerdata->motor_temperature = MS_U->Temperature-15; //according to documentation at endless sphere	
  // B10 and B11: 0
  ui8_tx_buffer [10] = 0;
  ui8_tx_buffer [11] = 0;

  // calculate CRC xor
  ui8_crc = 0;
  for (ui8_j = 1; ui8_j <= 11; ui8_j++)
  {
    ui8_crc ^= ui8_tx_buffer[ui8_j];
  }
  ui8_tx_buffer [6] = ui8_crc;

  // send the package over UART
  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&ui8_tx_buffer, 12);
}

/********************************************************************************************/
 // Process received package from the LCD
 //

 // see if we have a received package to be processed
void check_message(MotorState_t* MS_D, MotorParams_t* MP_D)
 {
  //printf("Byte recieved \r\n");
  // validation of the package data
   ui8_crc = 0;

   for (ui8_j = 0; ui8_j <= 12; ui8_j++)
   {
     if (ui8_j == 5) continue; // don't xor B5 (B7 in our case)
     ui8_crc ^= ui8_rx_buffer[ui8_j];
   }

   // check if end of message is OK
   if(ui8_rx_buffer[12]==0x0E){ // B11 is configurable via L1 and C4, not a good way to check that, C4 only in case the screen has the percentage option
	   // check if CRC is ok
   if (((ui8_crc ^ 10) == ui8_rx_buffer [5] || (ui8_crc ^ 27) == ui8_rx_buffer[5]) 	|| // some versions of CRC LCD5 (??)
	((ui8_crc ^ ui8_last_XOR) == ui8_rx_buffer [5])
	)
   { 
     lcd_configuration_variables.ui8_assist_level = ui8_rx_buffer [1] & 7;
     lcd_configuration_variables.ui8_light = ui8_rx_buffer [1]>>7 & 1;
     lcd_configuration_variables.ui8_motor_characteristic = ui8_rx_buffer [3];
     lcd_configuration_variables.ui8_wheel_size = ((ui8_rx_buffer [4] & 192) >> 6) | ((ui8_rx_buffer [2] & 7) << 2);
     lcd_configuration_variables.ui8_max_speed = (10 + ((ui8_rx_buffer [2] & 248) >> 3)) | (ui8_rx_buffer [4] & 32);
     lcd_configuration_variables.ui8_power_assist_control_mode = ui8_rx_buffer [4] & 8;
     lcd_configuration_variables.ui8_controller_max_current = (ui8_rx_buffer [7] & 15);
     MS_D->assist_level = lcd_configuration_variables.ui8_assist_level;
     MP_D->speedLimit = lcd_configuration_variables.ui8_max_speed;

    lcd_configuration_variables.ui8_p1 = ui8_rx_buffer[3];
    lcd_configuration_variables.ui8_p2 = ui8_rx_buffer[4] & 0x07;
    lcd_configuration_variables.ui8_p3 = ui8_rx_buffer[4] & 0x08;
    lcd_configuration_variables.ui8_p4 = ui8_rx_buffer[4] & 0x10;
    lcd_configuration_variables.ui8_p5 = ui8_rx_buffer[0];

    lcd_configuration_variables.ui8_c1 = (ui8_rx_buffer[6] & 0x38) >> 3;
    lcd_configuration_variables.ui8_c2 = (ui8_rx_buffer[6] & 0x37);
    lcd_configuration_variables.ui8_c4 = (ui8_rx_buffer[8] & 0xE0) >> 5;
    lcd_configuration_variables.ui8_c5 = (ui8_rx_buffer[7] & 0x0F);
    lcd_configuration_variables.ui8_c12 = (ui8_rx_buffer[9] & 0x0F);
    lcd_configuration_variables.ui8_c13 = (ui8_rx_buffer[10] & 0x1C) >> 2;
    lcd_configuration_variables.ui8_c14 = (ui8_rx_buffer[7] & 0x60) >> 5;
    if(lcd_configuration_variables.ui8_p1 != ui8_gear_ratio){
        ui8_gear_ratio=lcd_configuration_variables.ui8_p1/2;
    }

    MP_D->tics_higher_limit = WHEEL_CIRCUMFERENCE*5*3600/(6*ui8_gear_ratio*(MP_D->speedLimit+2)*10);
    MP_D->tics_lower_limit = WHEEL_CIRCUMFERENCE*5*3600/(6*ui8_gear_ratio*MP_D->speedLimit*10); //tics=wheelcirc*timerfrequency/(no. of hallevents per rev*gear-ratio*speedlimit)*3600/1000000

#ifdef ALLOW_DYNAMIC_CURRENT
    MP_D->battery_current_max = displaydata->p5 * 1000; // convert to milliamps
#endif
#ifdef ALLOW_DYNAMIC_REVERSE
    i8_reverse_flag = displaydata->c2 == 1 ? -1 : 1;
#endif
#ifdef ALLOW_DYNAMIC_REGEN
     MP_D->regen_current = (displaydata->c5 * 1000) / (CAL_I >> 8); 
#endif

     if(lcd_configuration_variables.ui8_light){
    	 HAL_GPIO_WritePin(LIGHT_GPIO_Port, LIGHT_Pin, GPIO_PIN_SET);
    	 HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
#ifdef BRAKELIGHT_IS_BACKLIGHT
			HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_SET);
#endif
     }
     else{
    	 HAL_GPIO_WritePin(LIGHT_GPIO_Port, LIGHT_Pin, GPIO_PIN_RESET);
    	 HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
#ifdef BRAKELIGHT_IS_BACKLIGHT
			HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, GPIO_PIN_RESET);
#endif
     }

     display_update(MS_D);
     check_recent(); //byte 1 contains the PAS level, that may be changed quite often. Better run only at system shutdown, due to limited possible write cycles to flash
   }//end CRC OK
   else{ //search for right last XOR

	   ui8_crc = 0;

	   for (ui8_j = 0; ui8_j <= 12; ui8_j++)
	   {
	     if (ui8_j == 5) continue; // don't xor B5 (B7 in our case)
	     ui8_crc ^= ui8_rx_buffer[ui8_j];
	   }
	   for (ui8_j = 0; ui8_j <= 50; ui8_j++)
	   {
	     if((ui8_crc ^ ui8_j) == ui8_rx_buffer [5]) ui8_last_XOR = ui8_j;
	   }
   }
   }// end EOT OK
   else{
	  //resyncronize the communication
	       CLEAR_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);
		   DMA1_Channel5->CNDTR=2;
		   SET_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);

		   if(ui8_rx_buffer[1]==0x0E ) { // values before 0x0e are unreliable as they are configurable on newer displays
	  	   CLEAR_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);
	  	   DMA1_Channel5->CNDTR=13;
	  	   SET_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);
		   }

   }
 }

//check if differences between initial values and recent values, store to emulated EEPROM if necessary
/*
EEPROM_KT_B0_B3
EEPROM_KT_B2_B4
EEPROM_KT_B6_B7
EEPROM_KT_B8_B9
EEPROM_KT_B1_B10
*/
//byte 1 contains the PAS level, that may be changed quite often. Better run only at system shutdown.
void check_recent(void){

	if(ui8_rx_buffer[0]!=ui8_rx_initial_buffer[0] || ui8_rx_buffer[3]!=ui8_rx_initial_buffer[3] ){
	    HAL_FLASH_Unlock();
	    EE_WriteVariable(EEPROM_KT_B0_B3,ui8_rx_buffer[0]<<8 | ui8_rx_buffer[3] );
	    HAL_FLASH_Lock();
	    ui8_rx_initial_buffer[0]=ui8_rx_buffer[0];
	    ui8_rx_initial_buffer[3]=ui8_rx_buffer[3];
	}

	if(ui8_rx_buffer[2]!=ui8_rx_initial_buffer[2] || ui8_rx_buffer[4]!=ui8_rx_initial_buffer[4] ){
	    HAL_FLASH_Unlock();
	    EE_WriteVariable(EEPROM_KT_B2_B4,ui8_rx_buffer[2]<<8 | ui8_rx_buffer[4] );
	    HAL_FLASH_Lock();
	    ui8_rx_initial_buffer[2]=ui8_rx_buffer[2];
	    ui8_rx_initial_buffer[4]=ui8_rx_buffer[4];
	}

	if(ui8_rx_buffer[6]!=ui8_rx_initial_buffer[6] || ui8_rx_buffer[7]!=ui8_rx_initial_buffer[7] ){
	    HAL_FLASH_Unlock();
	    EE_WriteVariable(EEPROM_KT_B6_B7,ui8_rx_buffer[6]<<8 | ui8_rx_buffer[7] );
	    HAL_FLASH_Lock();
	    ui8_rx_initial_buffer[6]=ui8_rx_buffer[6];
	    ui8_rx_initial_buffer[7]=ui8_rx_buffer[7];
	}

	if(ui8_rx_buffer[8]!=ui8_rx_initial_buffer[8] || ui8_rx_buffer[9]!=ui8_rx_initial_buffer[9] ){
	    HAL_FLASH_Unlock();
	    EE_WriteVariable(EEPROM_KT_B8_B9,ui8_rx_buffer[8]<<8 | ui8_rx_buffer[9] );
	    HAL_FLASH_Lock();
	    ui8_rx_initial_buffer[8]=ui8_rx_buffer[8];
	    ui8_rx_initial_buffer[9]=ui8_rx_buffer[9];
	}
//only check Byte 10
	if( ui8_rx_buffer[10]!=ui8_rx_initial_buffer[10] ){
	    HAL_FLASH_Unlock();
	    EE_WriteVariable(EEPROM_KT_B1_B10,ui8_rx_buffer[1]<<8 | ui8_rx_buffer[10] );
	    HAL_FLASH_Lock();
	    ui8_rx_initial_buffer[1]=ui8_rx_buffer[1];
	    ui8_rx_initial_buffer[10]=ui8_rx_buffer[10];
	}


}
