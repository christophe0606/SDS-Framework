#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "cg_status.h"

template<typename OUT,int outputSize>
class SDSSensor;

template<int outputSize>
class SDSSensor<uint8_t,outputSize>: 
GenericSource<uint8_t,outputSize>
{
public:
    SDSSensor(FIFOBase<uint8_t> &dst,
           sds_sensor_cg_connection_t *sdsConnection):
    GenericSource<uint8_t,outputSize>(dst),
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

        return(CG_SUCCESS);
    };

    int run() final{
        uint8_t *b=this->getWriteBuffer();

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

template<typename OUT1,int outputSize1,
         typename OUT2,int outputSize2>
class SDSTimedSensor;

template<int outputSize1,
         int outputSize2>
class SDSTimedSensor<uint8_t,outputSize1,
                     uint32_t,outputSize2>:public NodeBase
{
public:
    SDSTimedSensor(FIFOBase<uint8_t> &dst1,
                   FIFOBase<uint32_t> &dst2,
           sds_timed_sensor_cg_connection_t *sdsConnection):
    mDst1(dst1),
    mDst2(dst2),
    mSDS(sdsConnection){
    };

    /* Should not be used in async mode. Instead use the
       SDSAsyncSensor */
    int prepareForRunning() final
    {
        if (this->willOverflow1())
        {
           return(CG_BUFFER_OVERFLOW); // Skip execution
        }

        if (this->willOverflow2())
        {
           return(CG_BUFFER_OVERFLOW); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final{
        uint8_t *b=this->getWriteBuffer1();
        uint32_t *t=this->getWriteBuffer2();

        uint32_t remaining = outputSize1;
        uint32_t remainingTimestamp = outputSize2;

        while((remaining>0) && (remainingTimestamp>0))
        {
            if ((sdsGetCount(mSDS->sdsId)==0) && 
               (sdsGetCount(mSDS->sdsTimestampsId)==0))
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
            
            if (sdsGetCount(mSDS->sdsId)>0)
            {
              uint32_t num = sdsRead(mSDS->sdsId, b, remaining);
              b += num;
              remaining -= num;
            }

            if (sdsGetCount(mSDS->sdsTimestampsId)>0)
            {
              uint32_t num = sdsRead(mSDS->sdsTimestampsId, t, remainingTimestamp);
              t += num;
              remainingTimestamp -= num;
            }
            
        }
        return(CG_SUCCESS);
    };

protected:
    bool p;
    sds_timed_sensor_cg_connection_t *mSDS;

    uint8_t * getWriteBuffer1(int nb=outputSize1){return mDst1.getWriteBuffer(nb);};
    bool willOverflow1(int nb = outputSize1){return mDst1.willOverflowWith(nb);};

    uint32_t * getWriteBuffer2(int nb=outputSize2){return mDst2.getWriteBuffer(nb);};
    bool willOverflow2(int nb = outputSize2){return mDst2.willOverflowWith(nb);};

private:
    FIFOBase<uint8_t> &mDst1;
    FIFOBase<uint32_t> &mDst2;
};

template<typename OUT,int outputSize>
class SDSAsyncSensor;

template<int outputSize>
class SDSAsyncSensor<uint8_t,outputSize>: 
public NodeBase
{
public:
    SDSAsyncSensor(FIFOBase<uint8_t> &dst,
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
        uint8_t *b=this->getWriteBuffer();

        /* Try to read as much as we can save in the
           output FIFO */
        uint32_t num = sdsRead(mSDS->sdsId, b, mToRead);
        
        return(CG_SUCCESS);
    };

protected:
    uint8_t * getWriteBuffer(int nb=outputSize){return mDst.getWriteBuffer(nb);};

    bool willOverflow(int nb = outputSize){return mDst.willOverflowWith(nb);};

    int nbSamplesInWriteBuffer(){return(mDst.nbSamplesInFIFO());};

    sds_sensor_cg_connection_t *mSDS;
    uint32_t mToRead;
private:
    FIFOBase<uint8_t> &mDst;
};
