#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "cg_status.h"

/**************************************
 * 
 * SYNCHRONOUS NODES
 * 
 * ***********************************/

template<typename OUT,int outputSize>
class SDSSensor;

template<int outputSize>
class SDSSensor<uint8_t,outputSize>:public NodeBase
{
public:
    SDSSensor(FIFOBase<uint8_t> &dst,
           sds_sensor_cg_connection_t *sdsConnection,
           driftDelegate_t drift_delegate=nullptr,
           void *delegate_data=nullptr):
    mSDS(sdsConnection),
    m_drift_delegate(drift_delegate),
    m_delegate_data(delegate_data),
    mDst(dst){
        /* What to read in synchronous mode */
        mToRead = outputSize;
    };

    /* This function is only used in asynchronous mode */
    int prepareForRunning() final
    {
        /* What is the maximum amount of samples that can
           be read during the next run */
        mToRead = outputSize - this->nbSamplesInWriteBuffer();
        /* If overflow of output FIFO, execution is skipped */
        if (mToRead <= 0)
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        /*
           If no data in input SDS stream, we check for
           a cancel event and then skip the execution
        */
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

        /*
           Compute maximum amount of data that need
           to be generated in the output
           (based upon what's available in input and
           free in output)
        */
        uint32_t inSDS = sdsGetCount(mSDS->sdsId) ;
        mToRead = mToRead > inSDS ? inSDS : mToRead ;

        return(CG_SUCCESS);
    };

    int run() final{
        uint8_t *b=this->getWriteBuffer(mToRead);

        /* What need to be generated on output */
        uint32_t remaining = mToRead;

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
            

            uint32_t num;
            if (m_drift_delegate)
            {
               num=m_drift_delegate(mSDS->sdsId, b, remaining,m_delegate_data);
            }
            else
            {
               num = sdsRead(mSDS->sdsId, b, remaining);
            }
            b += num;
            remaining -= num;
            
        }
        return(CG_SUCCESS);
    };

protected:
    uint8_t * getWriteBuffer(int nb=outputSize){return mDst.getWriteBuffer(nb);};

    bool willOverflow(int nb = outputSize){return mDst.willOverflowWith(nb);};

    int nbSamplesInWriteBuffer(){return mDst.nbSamplesInFIFO();};
    
    bool p;
    sds_sensor_cg_connection_t *mSDS;
    driftDelegate_t m_drift_delegate;
    void *m_delegate_data;
    uint32_t mToRead;
private:
    FIFOBase<uint8_t> &mDst;
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
           sds_timed_sensor_cg_connection_t *sdsConnection,
           timedDriftDelegate_t drift_delegate=nullptr,
           void *delegate_data=nullptr):
    m_drift_delegate(drift_delegate),
    m_delegate_data(delegate_data),
    mDst1(dst1),
    mDst2(dst2),
    mSDS(sdsConnection){
        /* Number of samples to write in synchronous mode */
        mToReadData = outputSize1;
        mToReadTimestamps = outputSize2;
        ratio = outputSize1 / outputSize2;
    };

    
    int prepareForRunning() final
    {
        mToReadData = outputSize1 - this->nbSamplesInWriteBuffer1();
        mToReadTimestamps = outputSize2 - this->nbSamplesInWriteBuffer2();

       
        if ((mToReadData<=0) || (mToReadTimestamps<=0))
        {
           return(CG_SKIP_EXECUTION_ID_CODE); 
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

        uint32_t inDataSDS = sdsGetCount(mSDS->sdsId) ;
        uint32_t inTimestampSDS = sdsGetCount(mSDS->sdsTimestampsId) >> 2;

        mToReadData = mToReadData > inDataSDS ? inDataSDS : mToReadData;
        mToReadTimestamps = mToReadTimestamps > inTimestampSDS ? inTimestampSDS : mToReadTimestamps;

        /* Ensure the ratio between data and timestamps 
        is consistent*/
        uint32_t expected_nb_timestamps= mToReadData / ratio;
        if (expected_nb_timestamps > mToReadTimestamps)
        {
            mToReadData = mToReadTimestamps * ratio;
        }
        else if (expected_nb_timestamps < mToReadTimestamps)
        {
            mToReadTimestamps = expected_nb_timestamps;
        }
       

        if ((mToReadData<=0) || (mToReadTimestamps<=0))
        {
           return(CG_SKIP_EXECUTION_ID_CODE); 
        }

        return(CG_SUCCESS);
    };

    int run() final{
        uint8_t *b=this->getWriteBuffer1(mToReadData);
        uint32_t *t=this->getWriteBuffer2(mToReadTimestamps);

        uint32_t remaining = mToReadData;
        uint32_t remainingTimestamp = mToReadTimestamps;

        while((remaining>0) && (remainingTimestamp>0))
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
            
            
            uint32_t numData,numTimestamps; 
            if (m_drift_delegate)
            {
               m_drift_delegate(mSDS->sdsId, 
                                mSDS->sdsTimestampsId,
                                b, t,
                                remaining,remainingTimestamp,
                                ratio,
                                m_delegate_data,
                                &numData,&numTimestamps);
            }
            else 
            {
               numData = sdsRead(mSDS->sdsId, b, remaining);
               
               numTimestamps = sdsRead(mSDS->sdsTimestampsId, t, sizeof(uint32_t)*remainingTimestamp);
               
            }

            b += numData;
            remaining -= numData;

            t += (numTimestamps >> 2);
            remainingTimestamp -= (numTimestamps >>2);
            
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

    int nbSamplesInWriteBuffer1(){return mDst1.nbSamplesInFIFO();};
    int nbSamplesInWriteBuffer2(){return mDst2.nbSamplesInFIFO();};

    uint32_t mToReadData;
    uint32_t mToReadTimestamps;
    uint32_t ratio;

    timedDriftDelegate_t m_drift_delegate;
    void *m_delegate_data;

private:
    FIFOBase<uint8_t> &mDst1;
    FIFOBase<uint32_t> &mDst2;
};

