#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "sds_rec.h"
#include "cg_status.h"

template<typename OUT,int outputSize>
class SDSSource;

template<int outputSize>
class SDSSource<int8_t,outputSize>: 
GenericSource<int8_t,outputSize>
{
public:
    SDSSource(FIFOBase<int8_t> &dst,
           uint32_t event,
           uint32_t cancel_event,
           uint32_t timeout,
           sdsId_t sdsId):
    GenericSource<int8_t,outputSize>(dst),
    mEvent(event),
    mCancelEvent(cancel_event),
    mTimeout(timeout),
    mSdsId(sdsId){
    };

    /* Should not be used in async mode. Instead use the
       SDSAsyncSource */
    int prepareForRunning() final
    {
        if (this->willOverflow()
           )
        {
           return(CG_BUFFER_OVERFLOW); // Skip execution
        }

        return(0);
    };

    int run() final{
        int8_t *b=this->getWriteBuffer();

        uint32_t remaining = outputSize;

        while(remaining>0)
        {
            if (sdsGetCount(mSdsId)==0)
            {
               uint32_t flags = osThreadFlagsWait(mCancelEvent | mEvent, osFlagsWaitAny,mTimeout);
               if ((flags & osFlagsError) != 0U)
               {
                  return(CG_BUFFER_UNDERFLOW);
               }
               if (flags & mCancelEvent)
               {
                  return(CG_STOP_SCHEDULER);
               }
            }
            else
            {
                uint32_t flags = osThreadFlagsWait(mCancelEvent, osFlagsWaitAny,0);
                if (flags & mCancelEvent)
                {
                   return(CG_STOP_SCHEDULER);
                }
            }
            
            uint32_t num = sdsRead(mSdsId, b, remaining);
            b += num;
            remaining -= num;
            
        }
        return(CG_SUCCESS);
    };

protected:
    uint32_t mEvent;
    uint32_t mCancelEvent;
    uint32_t mTimeout;
    sdsId_t mSdsId;
};

template<typename OUT,int outputSize>
class SDSAsyncSource;

template<int outputSize>
class SDSAsyncSource<int8_t,outputSize>: 
public NodeBase
{
public:
    SDSAsyncSource(FIFOBase<int8_t> &dst,
           uint32_t cancel_event,
           sdsId_t sdsId):
    mCancelEvent(cancel_event),
    mSdsId(sdsId),
    mDst(dst){
    };

    int prepareForRunning() final
    {
        /* What is the maximum amount of samples that can
           be read during the next run */
        mToRead = outputSize - this->nbSamplesInWriteBuffer();
        if (mToRead <= 0)
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        if (sdsGetCount(mSdsId)==0)
        {
            uint32_t flags = osThreadFlagsWait(mCancelEvent, osFlagsWaitAny,0);
            if (flags & mCancelEvent)
            {
                   return(CG_STOP_SCHEDULER);
            }
            return(CG_SKIP_EXECUTION_ID_CODE);
        }

        return(CG_SUCCESS);
    };

    int run() final {
        int8_t *b=this->getWriteBuffer();

        /* Try to read as much as we can save in the
           output FIFO */
        uint32_t num = sdsRead(mSdsId, b, mToRead);
            
        return(CG_SUCCESS);
    };

protected:
    int8_t * getWriteBuffer(int nb=outputSize){return mDst.getWriteBuffer(nb);};

    bool willOverflow(int nb = outputSize){return mDst.willOverflowWith(nb);};

    int nbSamplesInWriteBuffer(){return(mDst.nbSamplesInFIFO());};

    uint32_t mCancelEvent;
    sdsId_t mSdsId;
    uint32_t mToRead;
private:
    FIFOBase<int8_t> &mDst;
};

#include <cstdio>
template<typename OUT,int inputSize>
class SDSRec;

template<int inputSize>
class SDSRec<int8_t,inputSize>: 
GenericSink<int8_t,inputSize>
{
public:
    SDSRec(FIFOBase<int8_t> &dst,
           const char *sensorName,
           uint8_t *recorderBuffer,
           uint32_t recorderBufferSize,
           uint32_t recorderThreshold):
    GenericSink<int8_t,inputSize>(dst)
    {
        mRecId = sdsRecOpen(sensorName,
                            recorderBuffer,
                            recorderBufferSize,
                            recorderThreshold);
    };

    ~SDSRec()
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