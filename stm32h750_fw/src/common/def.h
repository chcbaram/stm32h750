/*
 * def.h
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


#define _DEF_LED1                   0
#define _DEF_LED2                   1
#define _DEF_LED3                   2
#define _DEF_LED4                   3


#define _DEF_UART1                  0
#define _DEF_UART2                  1
#define _DEF_UART3                  2
#define _DEF_UART4                  3


#define _DEF_HIGH                   1
#define _DEF_LOW                    0


#define _DEF_LEFT                   0
#define _DEF_RIGHT                  1


#define _DEF_INPUT                  0
#define _DEF_INPUT_PULLUP           1
#define _DEF_INPUT_PULLDOWN         2
#define _DEF_OUTPUT                 3
#define _DEF_OUTPUT_PULLUP          4
#define _DEF_OUTPUT_PULLDOWN        5





#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))



#endif /* SRC_COMMON_DEF_H_ */
