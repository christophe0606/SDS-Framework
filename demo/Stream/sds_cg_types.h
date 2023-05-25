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
    uint32_t event;
    uint32_t cancel_event;
    uint32_t timeout;
    sdsId_t sdsId;
    sdsId_t sdsTimestampsId;
} sds_timed_sensor_cg_connection_t;


typedef struct {
    const char *sensorName;
    uint8_t *recorderBuffer;
    uint32_t recorderBufferSize;
    uint32_t recorderThreshold;
} sds_recorder_cg_connection_t;

typedef uint32_t (*driftDelegate_t) (sdsId_t id, 
                                     void *buf, 
                                     uint32_t buf_size,
                                     void *delegate_data);



typedef void (*timedDriftDelegate_t) (sdsId_t dataId, sdsId_t timestampId, 
                                      void *dataBuf,void *timestampBuf, 
                                      uint32_t dataBufSize,uint32_t timestampBufSize,
                                      uint32_t ratio,
                                      void *delegate_data,
                                      uint32_t *numData,uint32_t *numTimestamps);
#ifdef   __cplusplus
}
#endif

#endif