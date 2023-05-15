#ifndef _DEMO_NODES_H_ 
#define _DEMO_NODES_H_

#include "sds_cg_types.h"
#include "sds_sensor.hpp"
#include "sds_recorder.hpp"

#include "cg_status.h"

#include <cstdio>

template<typename IN, int inputSize>
class AccelerometerDisplay;

template<int inputSize>
class AccelerometerDisplay<accelerometerSample_t,inputSize>: 
public GenericSink<accelerometerSample_t, inputSize>
{
public:
    AccelerometerDisplay(FIFOBase<accelerometerSample_t> &src):GenericSink<accelerometerSample_t,inputSize>(src){};

    int prepareForRunning() final
    {
        if (this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final
    {
        accelerometerSample_t *b=this->getReadBuffer();
        for(int i=0;i<1;i++)
        {
            printf("x=%i, y=%i, z=%i\r\n",b[i].x,b[i].y,b[i].z);
       
        }
        return(0);
    };

};

template<typename IN, int inputSize,typename OUT,int outputSize>
class FormatAccelerometer;

template<int inputSize,int outputSize>
class FormatAccelerometer<int8_t,inputSize,
                          accelerometerSample_t,outputSize>: 
public GenericNode<int8_t,inputSize,accelerometerSample_t,outputSize>
{
public:
    FormatAccelerometer(FIFOBase<int8_t> &src,FIFOBase<accelerometerSample_t> &dst):
    GenericNode<int8_t,inputSize,accelerometerSample_t,outputSize>(src,dst){
    };

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };
    
    int run() final{
        int8_t *a=this->getReadBuffer();
        accelerometerSample_t *b=this->getWriteBuffer();

        uint32_t  buf[2];
        uint16_t *data_u16 = (uint16_t *)buf;

        int k=0;
        for(int i=0;i<inputSize;i+=6)
        {
            // Realign
            memcpy(buf,&a[i],6);

            b[k].x = data_u16[0];
            b[k].y = data_u16[1];
            b[k].z = data_u16[2];

            k++;
        }
        return(0);
    };


};
#endif