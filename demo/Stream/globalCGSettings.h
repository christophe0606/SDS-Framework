#ifndef _GLOBAL_CG_SETTINGS_H_ 
#define _GLOBAL_CG_SETTINGS_H_

#include <stdint.h>
#include "cmsis_os2.h"
#include "sds.h"

#include "sds_cg_types.h"


typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t z;
} accelerometerSample_t;

typedef struct {
  sds_sensor_cg_connection_t *sensorConn_accelerometer;
  sds_recorder_cg_connection_t *recConn_accelerometer;
} demoContext_t;
#endif