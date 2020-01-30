/*
 * ili9225.h
 *
 *  Created on: 2020. 1. 30.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_ILI9225_H_
#define SRC_COMMON_HW_INCLUDE_ILI9225_H_



#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"


#ifdef _USE_HW_ILI9225

#include "ili9225_regs.h"


#define _PIN_DEF_DC     0
#define _PIN_DEF_CS     1
#define _PIN_DEF_RST    2



bool ili9225Init();

void ili9225SetRotation(uint8_t mode);
void ili9225SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);


uint16_t ili9225GetWidth(void);
uint16_t ili9225GetHeight(void);


#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_ILI9225_H_ */
