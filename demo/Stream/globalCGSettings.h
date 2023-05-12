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
} demoContext_t;
#endif