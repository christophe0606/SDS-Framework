#ifndef _SDS_CG_TYPES_H_
#define _SDS_CG_TYPES_H_

#include "cmsis_os2.h"
#include "sds.h"

#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct {
    uint32_t event;
    uint32_t cancel_event;
    uint32_t timeout;
    sdsId_t sdsId;
} sds_sensor_cg_connection_t;


typedef struct {
    const char *sensorName;
    uint8_t *recorderBuffer;
    uint32_t recorderBufferSize;
    uint32_t recorderThreshold;
} sds_recorder_cg_connection_t;

#ifdef   __cplusplus
}
#endif

#endif