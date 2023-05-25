#include "drift_delegate.h"
#include <stdio.h>
typedef struct 
{
  uint32_t first_time;
  uint32_t current_count;
  uint32_t first_run;
  uint32_t accumulated;
  int32_t  excess_bytes;
  int32_t  removed;
  int32_t  t_removed;
} drift_delegate_data;

static drift_delegate_data data;

void *drift_data=(void*)&data;

void init_delegate_state(void *delegate_data)
{
   drift_delegate_data *d=(drift_delegate_data*)delegate_data;
   d->first_run=1;
   d->accumulated = 0;
   d->removed = 0;
   d->t_removed = 0;
} 

#if 0
void simple_timed_drift_correction (sdsId_t dataId, sdsId_t timestampId, 
                                    void *dataBuf,void *timestampBuf, 
                                    uint32_t dataBufSize,uint32_t timestampNbSamples,
                                    uint32_t ratio,
                                    void *delegate_data,
                                    uint32_t *numData,uint32_t *numTimestamps)
{
   *numData = sdsRead(dataId, dataBuf, dataBufSize);
              
   *numTimestamps = sdsRead(timestampId, timestampBuf, sizeof(uint32_t)*timestampNbSamples);

}
#else 
void simple_timed_drift_correction (sdsId_t dataId, sdsId_t timestampId, 
                                    void *dataBuf,void *timestampBuf, 
                                    uint32_t dataBufSize,uint32_t timestampNbSamples,
                                    uint32_t ratio,
                                    void *delegate_data,
                                    uint32_t *numData,uint32_t *numTimestamps)
{
              
   //*numTimestamps = sdsRead(timestampId, timestampBuf, sizeof(uint32_t)*timestampNbSamples);
   drift_delegate_data *d=(drift_delegate_data*)delegate_data;
   uint32_t *t = (uint32_t *)timestampBuf;
   if (d->first_run)
   {
       d->first_run = 0;
       /* Assume that data in both streams is always synchronized
          and coherent. If N is available in data stream, N/ratio is
          available in the other.
          So after sdsRead both stream will again be synchronized.
          In sensor handler : timestamp must be written before
          data since data may raise a FIFO high event
      */
       uint32_t numD = sdsRead(dataId, dataBuf, dataBufSize); 
       uint32_t numTime = sdsRead(timestampId, timestampBuf, sizeof(uint32_t)*timestampNbSamples); 
       d->first_time = t[(numTime>>2)-1];
       d->accumulated = 0;
       *numData = numD;
       *numTimestamps = numTime;
       return;
   }
   else 
   {
       uint32_t new_count=sdsGetCount(dataId);


       uint32_t total=0;
       uint32_t excess_bytes=0;


       uint32_t numD = sdsRead(dataId, dataBuf, dataBufSize); 
       uint32_t numTime = sdsRead(timestampId, timestampBuf, sizeof(uint32_t)*timestampNbSamples); 
       uint32_t new_time= t[(numTime>>2)-1];

       d->accumulated += numD;
       d->current_count=sdsGetCount(dataId);


       total = (d->accumulated + d->current_count);
       //float f = 1.0e3*total/(new_time-d->first_time)/4;
       //printf("%f\r\n",f);

       
       uint32_t expected = ((new_time-d->first_time)/10)<<2;

       if ((total-d->removed) > expected)
       {
          excess_bytes = (total - expected - d->removed);
       }

       /* We remove data in multiple of the timed block.
          So if timed block is N, we can only remove
          overhead by packet of N.
          It is the ratio that give the amount of byte
          per timestamp.
          When we remove a block of N data, we also need to
          remove the timestamp.
       */
       if (numD>=excess_bytes)
       {
            numD -= excess_bytes;
            d->removed += excess_bytes;
       }
       else
       {
          d->removed += numD;
          numD = 0;
       }

       uint32_t excess_timestamps;
       uint32_t numberOfTimeStamps = numTime >> 2;
       excess_timestamps = (d->removed / ratio) - d->t_removed;
       if (numberOfTimeStamps > excess_timestamps)
       {
         numTime -= (excess_timestamps<<2);
         d->t_removed += excess_timestamps;
       }
       else
       {
          d->t_removed += numberOfTimeStamps;
          numTime = 0;
       }
       
       
       *numData = numD;
       *numTimestamps = numTime;
       return;
   }
}
#endif
/*

Very simple example doing no correction and behaving as
the original node
*/
#if 0
uint32_t simple_drift_correction (sdsId_t id, 
                        void *buf, 
                        uint32_t buf_size,
                        void *delegate_data)
{
   drift_delegate_data *d=(drift_delegate_data*)delegate_data;
   uint32_t num = sdsRead(id, buf, buf_size);
   return(num);
}

#else
uint32_t simple_drift_correction (sdsId_t id, 
                                  void *buf, 
                                  uint32_t buf_size,
                                  void *delegate_data)
{
   drift_delegate_data *d=(drift_delegate_data*)delegate_data;
   if (d->first_run)
   {
       d->first_run = 0;
       uint32_t num = sdsRead(id, buf, buf_size); 
       d->first_time = osKernelGetTickCount();
       d->accumulated = 0;
       return(num);
   }
   else 
   {
       uint32_t new_time=osKernelGetTickCount();
       uint32_t new_count=sdsGetCount(id);


       uint32_t total=0;
       uint32_t excess_bytes=0;


       uint32_t num = sdsRead(id, buf, buf_size);
       d->accumulated += num;
       d->current_count=sdsGetCount(id);


       total = (d->accumulated + d->current_count);

       /*float f = 1.0e3*total/(new_time-d->first_time)/4;
       printf("%f\r\n",f);*/

       // Reference freq is 100 Hz
       uint32_t expected = ((new_time-d->first_time)/10)<<2;

       //printf("%d %d\r\n",total,expected);
       if ((total-d->removed) > expected)
       {
          excess_bytes = (total - expected - d->removed);
       }

       //printf("%d %d\r\n",num,excess_bytes);
       //printf("%d %d\r\n",num>>2, excess_bytes>>2);
       if (num>=excess_bytes)
       {
            num -= excess_bytes;
            //d->first_time = osKernelGetTickCount();
            //d->accumulated = 0;
            //d->removed = 0;
            d->removed += excess_bytes;
       }
       else
       {
          d->removed += num;
          num = 0;
       }
       
       
       return(num);
   }
}
#endif