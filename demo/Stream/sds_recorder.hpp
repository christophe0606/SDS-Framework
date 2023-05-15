#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "sds_rec.h"
#include "cg_status.h"

template<typename OUT,int inputSize>
class SDSRecorder;

template<int inputSize>
class SDSRecorder<int8_t,inputSize>: 
GenericSink<int8_t,inputSize>
{
public:
    SDSRecorder(FIFOBase<int8_t> &dst,
           sds_recorder_cg_connection_t *sdsConnection):
    GenericSink<int8_t,inputSize>(dst)
    {
        if (sdsConnection)
        {
        mRecId = sdsRecOpen(sdsConnection->sensorName,
                            sdsConnection->recorderBuffer,
                            sdsConnection->recorderBufferSize,
                            sdsConnection->recorderThreshold);
        }
        else
        {
            mRecId=NULL;
        }
    };

    ~SDSRecorder()
    {
        if (mRecId != NULL)
        {
           sdsRecClose(mRecId); 
        }
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        int8_t *b=this->getReadBuffer();
        if (mRecId == NULL)
        {
            return(CG_INIT_FAILURE);
        }

        uint32_t timestamp;
        timestamp = osKernelGetTickCount();

        uint32_t num = sdsRecWrite(mRecId, timestamp, b, inputSize);
        if (num != inputSize) {
            return(CG_BUFFER_OVERFLOW);
        }
        return(CG_SUCCESS);
    };

protected:
    sdsRecId_t mRecId;
};