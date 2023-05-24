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
} drift_delegate_data;

static drift_delegate_data data;

void *drift_data=(void*)&data;

void init_delegate_state(void *delegate_data)
{
   drift_delegate_data *d=(drift_delegate_data*)delegate_data;
   d->first_run=1;
   d->accumulated = 0;
   d->removed = 0;
} 
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
   float *out=(float*)buf;
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

       //float f = 1.0e3*total/(new_time-d->first_time);
       //printf("%f\r\n",f);

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