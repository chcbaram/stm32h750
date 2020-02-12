/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Sensor abstraction layer.
 */

/*
 * camera.h
 *
 *  Created on: 2020. 2. 12.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_CAMERA_H_
#define SRC_COMMON_HW_INCLUDE_CAMERA_H_


#include "def.h"



#include <stdint.h>
#include <stdarg.h>

#define OV7725_SLV_ADDR     (0x42/2)
#define OV2640_SLV_ADDR     (0x60/2)
#define MT9V034_SLV_ADDR    (0xB8/2)
#define LEPTON_SLV_ADDR     (0x54/2)
#define OV5640_SLV_ADDR     (0x78/2)
#define HM01B0_SLV_ADDR     (0x24/2)

// Chip ID registers.
#define OV_CHIP_ID          (0x0A)
#define OV5640_CHIP_ID      (0x300A)
#define ON_CHIP_ID          (0x00)
#define HIMAX_CHIP_ID       (0x0001)

#define OV9650_ID           (0x96)
#define OV2640_ID           (0x26)
#define OV7725_ID           (0x77)
#define OV5640_ID           (0x56)
#define MT9V034_ID          (0x13)
#define LEPTON_ID           (0x54)
#define HM01B0_ID           (0xB0)

typedef enum {
    PIXFORMAT_INVALID = 0,
    PIXFORMAT_BINARY,    // 1BPP/BINARY
    PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    PIXFORMAT_RGB565,    // 2BPP/RGB565
    PIXFORMAT_YUV422,    // 2BPP/YUV422
    PIXFORMAT_BAYER,     // 1BPP/RAW
    PIXFORMAT_JPEG,      // JPEG/COMPRESSED
} pixformat_t;

typedef enum {
    FRAMESIZE_INVALID = 0,
    // C/SIF Resolutions
    FRAMESIZE_QQCIF,    // 88x72
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_CIF,      // 352x288
    FRAMESIZE_QQSIF,    // 88x60
    FRAMESIZE_QSIF,     // 176x120
    FRAMESIZE_SIF,      // 352x240
    // VGA Resolutions
    FRAMESIZE_QQQQVGA,  // 40x30
    FRAMESIZE_QQQVGA,   // 80x60
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_HQQQVGA,  // 60x40
    FRAMESIZE_HQQVGA,   // 120x80
    FRAMESIZE_HQVGA,    // 240x160
    // FFT Resolutions
    FRAMESIZE_64X32,    // 64x32
    FRAMESIZE_64X64,    // 64x64
    FRAMESIZE_128X64,   // 128x64
    FRAMESIZE_128X128,  // 128x128
    // Other
    FRAMESIZE_LCD,      // 128x160
    FRAMESIZE_QQVGA2,   // 128x160
    FRAMESIZE_WVGA,     // 720x480
    FRAMESIZE_WVGA2,    // 752x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_FHD,      // 1920x1080
    FRAMESIZE_QHD,      // 2560x1440
    FRAMESIZE_QXGA,     // 2048x1536
    FRAMESIZE_WQXGA,    // 2560x1600
    FRAMESIZE_WQXGA2,   // 2592x1944
} framesize_t;

typedef enum {
    FRAMERATE_2FPS =0x9F,
    FRAMERATE_8FPS =0x87,
    FRAMERATE_15FPS=0x83,
    FRAMERATE_30FPS=0x81,
    FRAMERATE_60FPS=0x80,
} framerate_t;

typedef enum {
    GAINCEILING_2X,
    GAINCEILING_4X,
    GAINCEILING_8X,
    GAINCEILING_16X,
    GAINCEILING_32X,
    GAINCEILING_64X,
    GAINCEILING_128X,
} gainceiling_t;

typedef enum {
    SDE_NORMAL,
    SDE_NEGATIVE,
} sde_t;

typedef enum {
    ATTR_CONTRAST=0,
    ATTR_BRIGHTNESS,
    ATTR_SATURATION,
    ATTR_GAINCEILING,
} sensor_attr_t;

typedef enum {
    ACTIVE_LOW,
    ACTIVE_HIGH
} polarity_t;

typedef enum {
    IOCTL_SET_TRIGGERED_MODE,
    IOCTL_GET_TRIGGERED_MODE,
    IOCTL_LEPTON_GET_WIDTH,
    IOCTL_LEPTON_GET_HEIGHT,
    IOCTL_LEPTON_GET_RADIOMETRY,
    IOCTL_LEPTON_GET_REFRESH,
    IOCTL_LEPTON_GET_RESOLUTION,
    IOCTL_LEPTON_RUN_COMMAND,
    IOCTL_LEPTON_SET_ATTRIBUTE,
    IOCTL_LEPTON_GET_ATTRIBUTE,
    IOCTL_LEPTON_GET_FPA_TEMPERATURE,
    IOCTL_LEPTON_GET_AUX_TEMPERATURE,
    IOCTL_LEPTON_SET_MEASUREMENT_MODE,
    IOCTL_LEPTON_GET_MEASUREMENT_MODE,
    IOCTL_LEPTON_SET_MEASUREMENT_RANGE,
    IOCTL_LEPTON_GET_MEASUREMENT_RANGE
} ioctl_t;

#define SENSOR_HW_FLAGS_VSYNC        (0) // vertical sync polarity.
#define SENSOR_HW_FLAGS_HSYNC        (1) // horizontal sync polarity.
#define SENSOR_HW_FLAGS_PIXCK        (2) // pixel clock edge.
#define SENSOR_HW_FLAGS_FSYNC        (3) // hardware frame sync.
#define SENSOR_HW_FLAGS_JPEGE        (4) // hardware JPEG encoder.
#define SENSOR_HW_FLAGS_GET(s, x)    ((s)->hw_flags &  (1<<x))
#define SENSOR_HW_FLAGS_SET(s, x, v) ((s)->hw_flags |= (v<<x))
#define SENSOR_HW_FLAGS_CLR(s, x)    ((s)->hw_flags &= ~(1<<x))

//typedef bool (*streaming_cb_t)(image_t *image);

typedef struct _camera camera_t;
typedef struct _camera {
    uint8_t  chip_id;           // Sensor ID.
    uint8_t  slv_addr;          // Sensor I2C slave address.
    uint16_t gs_bpp;            // Grayscale bytes per pixel.
    uint32_t hw_flags;          // Hardware flags (clock polarities/hw capabilities)
    const uint16_t *color_palette;    // Color palette used for color lookup.

    uint32_t vsync_pin;         // VSYNC GPIO output pin.
    //GPIO_TypeDef *vsync_gpio;   // VSYNC GPIO output port.

    polarity_t pwdn_pol;        // PWDN polarity (TODO move to hw_flags)
    polarity_t reset_pol;       // Reset polarity (TODO move to hw_flags)

    // Sensor state
    sde_t sde;                  // Special digital effects
    pixformat_t pixformat;      // Pixel format
    framesize_t framesize;      // Frame size
    framerate_t framerate;      // Frame rate
    gainceiling_t gainceiling;  // AGC gainceiling

    // Sensor function pointers
    int  (*reset)               (camera_t *sensor);
    int  (*sleep)               (camera_t *sensor, int enable);
    int  (*read_reg)            (camera_t *sensor, uint16_t reg_addr);
    int  (*write_reg)           (camera_t *sensor, uint16_t reg_addr, uint16_t reg_data);
    int  (*set_pixformat)       (camera_t *sensor, pixformat_t pixformat);
    int  (*set_framesize)       (camera_t *sensor, framesize_t framesize);
    int  (*set_framerate)       (camera_t *sensor, framerate_t framerate);
    int  (*set_contrast)        (camera_t *sensor, int level);
    int  (*set_brightness)      (camera_t *sensor, int level);
    int  (*set_saturation)      (camera_t *sensor, int level);
    int  (*set_gainceiling)     (camera_t *sensor, gainceiling_t gainceiling);
    int  (*set_quality)         (camera_t *sensor, int quality);
    int  (*set_colorbar)        (camera_t *sensor, int enable);
    int  (*set_auto_gain)       (camera_t *sensor, int enable, float gain_db, float gain_db_ceiling);
    int  (*get_gain_db)         (camera_t *sensor, float *gain_db);
    int  (*set_auto_exposure)   (camera_t *sensor, int enable, int exposure_us);
    int  (*get_exposure_us)     (camera_t *sensor, int *exposure_us);
    int  (*set_auto_whitebal)   (camera_t *sensor, int enable, float r_gain_db, float g_gain_db, float b_gain_db);
    int  (*get_rgb_gain_db)     (camera_t *sensor, float *r_gain_db, float *g_gain_db, float *b_gain_db);
    int  (*set_hmirror)         (camera_t *sensor, int enable);
    int  (*set_vflip)           (camera_t *sensor, int enable);
    int  (*set_special_effect)  (camera_t *sensor, sde_t sde);
    int  (*set_lens_correction) (camera_t *sensor, int enable, int radi, int coef);
    int  (*ioctl)               (camera_t *sensor, int request, va_list ap);
    //int  (*snapshot)            (camera_t *sensor, image_t *image, streaming_cb_t streaming_cb);
} camera_t;

// Resolution table
extern const int resolution[][2];

// Initialize the sensor hardware and probe the image sensor.
bool cameraInit(void);

// Initialize the sensor state.
void cameraInit0();

// Reset the sensor to its default state.


// Return sensor PID.
int cameraGetId();

// Sleep mode.
int cameraSleep(int enable);

// Shutdown mode.
int cameraShutdown(int enable);

// Read a sensor register.
int cameraReadReg(uint16_t reg_addr);

// Write a sensor register.
int cameraWriteReg(uint16_t reg_addr, uint16_t reg_data);

// Set the sensor pixel format.
int cameraSetPixformat(pixformat_t pixformat);

// Set the sensor frame size.
int cameraSetFramesize(framesize_t framesize);

// Set the sensor frame rate.
int cameraSetFramerate(framerate_t framerate);

// Set window size.
int cameraSetWindowing(int x, int y, int w, int h);

// Set the sensor contrast level (from -3 to +3).
int cameraSetContrast(int level);

// Set the sensor brightness level (from -3 to +3).
int cameraSetBrightness(int level);

// Set the sensor saturation level (from -3 to +3).
int cameraSetSaturation(int level);

// Set the sensor AGC gain ceiling.
// Note: This function has no effect when AGC (Automatic Gain Control) is disabled.
int cameraSetGainceiling(gainceiling_t gainceiling);

// Set the quantization scale factor, controls JPEG quality (quality 0-255).
int cameraSetQuality(int qs);

// Enable/disable the colorbar mode.
int cameraSetColorbar(int enable);

// Enable auto gain or set value manually.
int cameraSetAutoGain(int enable, float gain_db, float gain_db_ceiling);

// Get the gain value.
int cameraGetGainDb(float *gain_db);

// Enable auto exposure or set value manually.
int cameraSetAutoExposure(int enable, int exposure_us);

// Get the exposure value.
int cameraGetExposure_us(int *get_exposure_us);

// Enable auto white balance or set value manually.
int cameraSetAutoWhitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db);

// Get the rgb gain values.
int cameraGetRgbGainDb(float *r_gain_db, float *g_gain_db, float *b_gain_db);

// Enable/disable the hmirror mode.
int cameraSetHmirror(int enable);

// Enable/disable the vflip mode.
int cameraSetVflip(int enable);

// Set special digital effects (SDE).
int cameraSetSpecialEffect(sde_t sde);

// Set lens shading correction
int cameraSetLensCorrection(int enable, int radi, int coef);

// IOCTL function
int cameraIoctl(int request, ...);

// Set vsync output pin
//int cameraSetVsyncOutput(GPIO_TypeDef *gpio, uint32_t pin);

// Set color palette
int cameraSetColorPalette(const uint16_t *color_palette);

// Get color palette
const uint16_t *cameraGetColorPalette();



#define CAMERA_MODE_CONTINUOUS         DCMI_MODE_CONTINUOUS
#define CAMERA_MODE_SNAPSHOT           DCMI_MODE_SNAPSHOT


bool cameraReset();
bool cameraIsAvailble(void);
bool cameraStart(uint8_t *pBff, uint32_t Mode);
bool cameraStop(void);
bool cameraSuspend(void);
bool cameraResume(void);


#endif /* SRC_COMMON_HW_INCLUDE_CAMERA_H_ */
