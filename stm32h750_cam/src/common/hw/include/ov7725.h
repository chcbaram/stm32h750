/*
 * ov7725.h
 *
 *  Created on: 2020. 2. 12.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_OV7725_H_
#define SRC_COMMON_HW_INCLUDE_OV7725_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#include "camera.h"

#ifdef _USE_HW_OV7725


bool ov7725Init(void);
bool ov7725Open(camera_t *sensor);

#endif


#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_OV7725_H_ */
