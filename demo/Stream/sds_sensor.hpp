#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "cg_status.h"

#include <cstdio>

template<typename OUT,int outputSize>
class SDSSensor;

template<int outputSize>
class SDSSensor<int8_t,outputSize>: 
GenericSource<int8_t,outputSize>
{
public:
    SDSSensor(FIFOBase<int8_t> &dst,
           sds_sensor_cg_connection_t *sdsConnection):
    GenericSource<int8_t,outputSize>(dst),
    mSDS(sdsConnection){
    };

    /* Should not be used in async mode. Instead use the
       SDSAsyncSensor */
    int prepareForRunning() final
    {
        if (this->willOverflow())
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
            if (sdsGetCount(mSDS->sdsId)==0)
            {
               uint32_t flags = osThreadFlagsWait(mSDS->cancel_event | mSDS->event, osFlagsWaitAny,mSDS->timeout);
               if ((flags & osFlagsError) != 0U)
               {
                  return(CG_BUFFER_UNDERFLOW);
               }
               if (flags & mSDS->cancel_event)
               {
                  return(CG_STOP_SCHEDULER);
               }
            }
            else
            {
                uint32_t flags = osThreadFlagsWait(mSDS->cancel_event, osFlagsWaitAny,0);
                if ((flags & osFlagsError) == 0U)
                {
                    if (flags & mSDS->cancel_event)
                    {
                       return(CG_STOP_SCHEDULER);
                    }
                }
                else 
                {
                    // osFlagsErrorResource means no cancel event
                    // has been received
                    if ((flags & osFlagsErrorResource) == 0U)
                    {
                        return(CG_OS_ERROR);
                    }
                }
            }
            
            uint32_t num = sdsRead(mSDS->sdsId, b, remaining);
            b += num;
            remaining -= num;
            
        }
        return(CG_SUCCESS);
    };

protected:
    bool p;
    sds_sensor_cg_connection_t *mSDS;
};

template<typename OUT,int outputSize>
class SDSAsyncSensor;

template<int outputSize>
class SDSAsyncSensor<int8_t,outputSize>: 
public NodeBase
{
public:
    SDSAsyncSensor(FIFOBase<int8_t> &dst,
           sds_sensor_cg_connection_t *sdsConnection):
    mDst(dst),
    mSDS(sdsConnection){
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

        if (sdsGetCount(mSDS->sdsId)==0)
        {
            uint32_t flags = osThreadFlagsWait(mSDS->cancel_event, osFlagsWaitAny,0);
            if ((flags & osFlagsError) == 0U)
            {
                if (flags & mSDS->cancel_event)
                {
                   return(CG_STOP_SCHEDULER);
                }
            }
            else 
            {
                // osFlagsErrorResource means no cancel event
                // has been received
                if ((flags & osFlagsErrorResource) == 0U)
                {
                    return(CG_OS_ERROR);
                }
            }
            return(CG_SKIP_EXECUTION_ID_CODE);
        }

        return(CG_SUCCESS);
    };

    int run() final {
        int8_t *b=this->getWriteBuffer();

        /* Try to read as much as we can save in the
           output FIFO */
        uint32_t num = sdsRead(mSDS->sdsId, b, mToRead);
            
        return(CG_SUCCESS);
    };

protected:
    int8_t * getWriteBuffer(int nb=outputSize){return mDst.getWriteBuffer(nb);};

    bool willOverflow(int nb = outputSize){return mDst.willOverflowWith(nb);};

    int nbSamplesInWriteBuffer(){return(mDst.nbSamplesInFIFO());};

    sds_sensor_cg_connection_t *mSDS;
    uint32_t mToRead;
private:
    FIFOBase<int8_t> &mDst;
};
