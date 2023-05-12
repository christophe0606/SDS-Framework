#pragma once

#include "cmsis_os2.h"
#include "sds.h"
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

    int prepareForRunning() final
    {
        if (this->willOverflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
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
class SDSRec;

template<int outputSize>
class SDSRec<int8_t,outputSize>: 
GenericSink<int8_t,outputSize>
{
public:
    SDSRec(FIFOBase<int8_t> &dst):
    GenericSink<int8_t,outputSize>(dst)
    {
        /* recId_accelerometer = sdsRecOpen("Accelerometer",
                                      recBuf_accelerometer,
                                      sizeof(recBuf_accelerometer),
                                      REC_IO_THRESHOLD_ACCELEROMETER);
*/
    };

    ~SDSRec()
    {
        /* sdsRecClose(recId_accelerometer); */
    }

    int prepareForRunning() final
    {
        if (this->willOverflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        int8_t *b=this->getReadBuffer();

       /* num = sdsRecWrite(recId_temperatureSensor, timestamp, sensorBuf, buf_size);
          if (num != buf_size) {
            printf("%s: Recorder write failed\r\n", sensorConfig_temperatureSensor->name);
          }
          */
        return(CG_SUCCESS);
    };

protected:
    
};