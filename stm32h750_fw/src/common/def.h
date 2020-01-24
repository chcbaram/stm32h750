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


#define _DEF_LED1         0
#define _DEF_LED2         1
#define _DEF_LED3         2
#define _DEF_LED4         3


#define _DEF_UART1        0
#define _DEF_UART2        1
#define _DEF_UART3        2
#define _DEF_UART4        3






#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))



#endif /* SRC_COMMON_DEF_H_ */
