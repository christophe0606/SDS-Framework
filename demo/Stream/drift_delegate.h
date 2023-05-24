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

#ifdef   __cplusplus
}
#endif

#endif