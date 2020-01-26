/*
 * bsp.h
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "def.h"
#include "stm32h7xx_hal.h"


#define logPrintf(...)    printf(__VA_ARGS__)


void bspInit(void);
void bspDeInit(void);


extern void delay(uint32_t delay_ms);
extern uint32_t millis(void);
extern void Error_Handler(void);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BSP_BSP_H_ */
