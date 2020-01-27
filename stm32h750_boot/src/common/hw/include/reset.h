/*
 * reset.h
 *
 *  Created on: 2020. 1. 27.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_RESET_H_
#define SRC_COMMON_HW_INCLUDE_RESET_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_RESET


#define RESET_BOOT_CNT    HW_RESET_BOOT_CNT
#define RESET_BOOT_MODE   HW_RESET_BOOT_MODE
#define RESET_BOOT_SRC    HW_RESET_BOOT_SRC



void resetInit(void);
void resetLog(void);
void resetRunSoftReset(void);
void resetClearFlag(void);
uint8_t resetGetStatus(void);
uint8_t resetGetBits(void);
void resetWaitCount(void);
uint8_t resetGetCount(void);
void resetToSysBoot(void);


#endif

#ifdef __cplusplus
 }
#endif



#endif /* SRC_COMMON_HW_INCLUDE_RESET_H_ */
