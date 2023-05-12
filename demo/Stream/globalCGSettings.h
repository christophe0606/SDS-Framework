#ifndef _GLOBAL_CG_SETTINGS_H_ 
#define _GLOBAL_CG_SETTINGS_H_

#include <stdint.h>
#include "cmsis_os2.h"
#include "sds.h"

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t z;
} accelerometerSample_t;

typedef struct {
  sdsId_t accId;
  uint8_t *recBuf_accelerometer;
  uint32_t recBufSize_accelerometer;
  uint32_t recorderAccThreshold;
} demoContext_t;
#endif