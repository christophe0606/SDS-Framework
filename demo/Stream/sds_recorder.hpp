#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "sds_rec.h"
#include "cg_status.h"

template<typename IN,int inputSize>
class SDSRecorder: GenericSink<IN,inputSize>
{
public:
    SDSRecorder(FIFOBase<IN> &src,
           sds_recorder_cg_connection_t *sdsConnection):
    GenericSink<IN,inputSize>(src)
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
        IN *b=this->getReadBuffer();
        if (mRecId == NULL)
        {
            return(CG_INIT_FAILURE);
        }

        uint32_t timestamp;
        timestamp = osKernelGetTickCount();

        uint32_t num = sdsRecWrite(mRecId, timestamp, b, inputSize*sizeof(IN));
        if (num != inputSize*sizeof(IN)) {
            return(CG_BUFFER_OVERFLOW);
        }
        return(CG_SUCCESS);
    };

protected:
    sdsRecId_t mRecId;
};

template<typename IN1,int inputSize1,
         typename IN2,int inputSize2>
class SDSTimedRecorder;

template<typename IN,int inputSize1,int inputSize2>
class SDSTimedRecorder<IN,inputSize1,
                       uint32_t,inputSize2>:public NodeBase
{
public:
    SDSTimedRecorder(FIFOBase<IN> &src1,
                     FIFOBase<uint32_t> &src2,
           sds_recorder_cg_connection_t *sdsConnection):
    mSrc1(src1),mSrc2(src2) 
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

    ~SDSTimedRecorder()
    {
        if (mRecId != NULL)
        {
           sdsRecClose(mRecId); 
        }
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow1())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        if (this->willUnderflow2())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        IN *b=this->getReadBuffer1();
        uint32_t *timing=this->getReadBuffer2();
        if (mRecId == NULL)
        {
            return(CG_INIT_FAILURE);
        }

        const int nb_blocks = inputSize1 / inputSize2;
        for(int blockNb=0;blockNb < nb_blocks;blockNb++)
        {
            uint32_t num = sdsRecWrite(mRecId, timing[blockNb], (void *)b, sizeof(IN)*inputSize2);
            if (num != sizeof(IN)*inputSize2) 
            {
               return(CG_BUFFER_OVERFLOW);
            }
            b += inputSize2;
        }

        
        return(CG_SUCCESS);
    };

protected:
    sdsRecId_t mRecId;

    IN * getReadBuffer1(int nb=inputSize1){return mSrc1.getReadBuffer(nb);};
    bool willUnderflow1(int nb = inputSize1){return mSrc1.willUnderflowWith(nb);};

    uint32_t * getReadBuffer2(int nb=inputSize2){return mSrc2.getReadBuffer(nb);};
    bool willUnderflow2(int nb = inputSize2){return mSrc2.willUnderflowWith(nb);};

private:
    FIFOBase<IN> &mSrc1;
    FIFOBase<uint32_t> &mSrc2;
};