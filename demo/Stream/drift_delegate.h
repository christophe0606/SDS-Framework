#ifndef _DRIFT_DELEGATE_H_
#define _DRIFT_DELEGATE_H_

#include "sds_cg_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

extern void *drift_data;

extern void init_delegate_state(void *delegate_data);


extern uint32_t simple_drift_correction (sdsId_t id, 
                                         void *buf, 
                                         uint32_t buf_size,
                                         void *delegate_data);

extern void simple_timed_drift_correction (sdsId_t dataId, sdsId_t timestampId, 
                                           void *dataBuf,void *timestampBuf, 
                                           uint32_t dataBufSize,uint32_t timestampNbSamples,
                                           uint32_t ratio,
                                           void *delegate_data,
                                           uint32_t *numData,uint32_t *numTimestamps);

#ifdef   __cplusplus
}
#endif

#endif