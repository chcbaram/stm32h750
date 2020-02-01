/*
 * hw_def.h
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"



#define _HW_DEF_RTOS_MEM_SIZE(x)              ((x)/4)

#define _HW_DEF_RTOS_THREAD_PRI_MAIN          osPriorityNormal

#define _HW_DEF_RTOS_THREAD_MEM_MAIN          _HW_DEF_RTOS_MEM_SIZE(6*1024)



#define _USE_HW_RTOS
#define _USE_HW_DELAY
#define _USE_HW_MILLIS
#define _USE_HW_QSPI
#define _USE_HW_FLASH
#define _USE_HW_SD
#define _USE_HW_FATFS


#define _USE_HW_RTC
#define      HW_RTC_CFG_DATA_1      RTC_BKP_DR1
#define      HW_RTC_CFG_DATA_2      RTC_BKP_DR2

#define _USE_HW_RESET
#define      HW_RESET_BOOT_CNT      RTC_BKP_DR3
#define      HW_RESET_BOOT_MODE     RTC_BKP_DR4
#define      HW_RESET_BOOT_SRC      RTC_BKP_DR5


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2

#define _USE_HW_VCP
#define _USE_HW_USB
#define      HW_USE_CDC             1
#define      HW_USE_MSC             0

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         1

#define _USE_HW_PWM
#define      HW_PWM_MAX_CH          1

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          1

#define _USE_HW_ILI9225

#define _USE_HW_LCD
#define      HW_LCD_WIDTH           220
#define      HW_LCD_HEIGHT          176

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       10

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      8

#define _USE_HW_ADC
#define      HW_ADC_MAX_CH          2

#define _USE_HW_JOYPAD


#define _USE_HW_CMDIF
#define      HW_CMDIF_LIST_MAX              32
#define      HW_CMDIF_CMD_STR_MAX           16
#define      HW_CMDIF_CMD_BUF_LENGTH        128




#define _PIN_GPIO_SDCARD_DETECT     0


#define _HW_DEF_RTC_BOOT_RESET      RTC_BKP_DR3
#define _HW_DEF_RTC_BOOT_MODE       RTC_BKP_DR4
#define _HW_DEF_RTC_RESET_SRC       RTC_BKP_DR5


#define _DEF_HW_BTN_A               0
#define _DEF_HW_BTN_B               1
#define _DEF_HW_BTN_SELECT          2
#define _DEF_HW_BTN_START           3
#define _DEF_HW_BTN_MENU            4
#define _DEF_HW_BTN_HOME            5
#define _DEF_HW_BTN_LEFT            6
#define _DEF_HW_BTN_RIGHT           7
#define _DEF_HW_BTN_UP              8
#define _DEF_HW_BTN_DOWN            9



#endif /* SRC_HW_HW_DEF_H_ */
