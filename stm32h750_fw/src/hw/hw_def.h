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


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2

#define _USE_HW_VCP
#define _USE_HW_USB
#define      HW_USE_CDC             0
#define      HW_USE_MSC             1

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         1

#define _USE_HW_PWM
#define      HW_PWM_MAX_CH          1

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          1

#define _USE_HW_ILI9225


#define _USE_HW_CMDIF
#define      HW_CMDIF_LIST_MAX              32
#define      HW_CMDIF_CMD_STR_MAX           16
#define      HW_CMDIF_CMD_BUF_LENGTH        128




#define _PIN_GPIO_SDCARD_DETECT     0


#define _HW_DEF_RTC_BOOT_RESET      RTC_BKP_DR3
#define _HW_DEF_RTC_BOOT_MODE       RTC_BKP_DR4
#define _HW_DEF_RTC_RESET_SRC       RTC_BKP_DR5



#endif /* SRC_HW_HW_DEF_H_ */
