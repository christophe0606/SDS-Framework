#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "sds_rec.h"
#include "cg_status.h"

template<typename IN,int inputSize>
class SDSRecorder;

template<int inputSize>
class SDSRecorder<uint8_t,inputSize>: 
GenericSink<uint8_t,inputSize>
{
public:
    SDSRecorder(FIFOBase<uint8_t> &src,
           sds_recorder_cg_connection_t *sdsConnection):
    GenericSink<uint8_t,inputSize>(src)
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
        uint8_t *b=this->getReadBuffer();
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

template<typename IN1,int inputSize1,
         typename IN2,int inputSize2>
class SDSTimedRecorder;

template<int inputSize1,int inputSize2>
class SDSTimedRecorder<uint8_t,inputSize1,
                       uint32_t,inputSize2>:public NodeBase
{
public:
    SDSTimedRecorder(FIFOBase<uint8_t> &src1,
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
        uint8_t *b=this->getReadBuffer1();
        uint32_t *timing=this->getReadBuffer2();
        if (mRecId == NULL)
        {
            return(CG_INIT_FAILURE);
        }

        const int nb_blocks = inputSize1 / inputSize2;
        for(int blockNb=0;blockNb < nb_blocks;blockNb++)
        {
            uint32_t num = sdsRecWrite(mRecId, timing[blockNb], b, inputSize2);
            if (num != inputSize2) 
            {
               return(CG_BUFFER_OVERFLOW);
            }
            b += inputSize2;
        }

        
        return(CG_SUCCESS);
    };

protected:
    sdsRecId_t mRecId;

    uint8_t * getReadBuffer1(int nb=inputSize1){return mSrc1.getReadBuffer(nb);};
    bool willUnderflow1(int nb = inputSize1){return mSrc1.willUnderflowWith(nb);};

    uint32_t * getReadBuffer2(int nb=inputSize2){return mSrc2.getReadBuffer(nb);};
    bool willUnderflow2(int nb = inputSize2){return mSrc2.willUnderflowWith(nb);};

private:
    FIFOBase<uint8_t> &mSrc1;
    FIFOBase<uint32_t> &mSrc2;
};