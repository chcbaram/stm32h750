/*
 * speaker.h
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_SPEAKER_H_
#define SRC_COMMON_HW_INCLUDE_SPEAKER_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_SPEAKER


bool speakerInit(void);
void speakerEnable(void);
void speakerDisable(void);
void speakerSetVolume(uint8_t volume_data);
uint8_t speakerGetVolume(void);
void speakerStart(uint32_t hz);
void speakerReStart(void);
void speakerStop(void);
uint32_t speakerAvailable(void);
uint32_t speakerGetBufLength(void);
void speakerPutch(uint8_t data);
void speakerWrite(uint8_t *p_data, uint32_t length);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_SPEAKER_H_ */
