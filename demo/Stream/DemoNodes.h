#ifndef _DEMO_NODES_H_ 
#define _DEMO_NODES_H_

#include "sds_cg_types.h"
#include "sds_sensor.hpp"
#include "sds_recorder.hpp"

#include "cg_status.h"

#include <cstdio>

template<typename IN, int inputSize>
class VectorDisplay;

template<int inputSize>
class VectorDisplay<vectorSample_t,inputSize>: 
public GenericSink<vectorSample_t, inputSize>
{
public:
    VectorDisplay(FIFOBase<vectorSample_t> &src,const char* name):
    GenericSink<vectorSample_t,inputSize>(src),mName(name){};

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final
    {
        vectorSample_t *b=this->getReadBuffer();
        for(int i=0;i<1;i++)
        {
            printf("%s: x=%i, y=%i, z=%i\r\n",mName,b[i].x,b[i].y,b[i].z);
       
        }
        return(0);
    };

protected:
    const char* mName;
};

template<typename IN, int inputSize,typename OUT,int outputSize>
class FormatVector;

template<int inputSize,int outputSize>
class FormatVector<int8_t,inputSize,
                          vectorSample_t,outputSize>: 
public GenericNode<int8_t,inputSize,vectorSample_t,outputSize>
{
public:
    FormatVector(FIFOBase<int8_t> &src,FIFOBase<vectorSample_t> &dst):
    GenericNode<int8_t,inputSize,vectorSample_t,outputSize>(src,dst){
    };

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };
    
    int run() final{
        int8_t *a=this->getReadBuffer();
        vectorSample_t *b=this->getWriteBuffer();

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


template<typename IN, int inputSize,typename OUT,int outputSize>
class FormatTemperature;

template<>
class FormatTemperature<int8_t,4,
                        float,1>: 
public GenericNode<int8_t,4,
                   float,1>
{
public:
    FormatTemperature(FIFOBase<int8_t> &src,
                 FIFOBase<float> &dst):
    GenericNode<int8_t,4,
                float,1>(src,dst){
    };

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };
    
    int run() final{
        int8_t *a=this->getReadBuffer();
        float *b=this->getWriteBuffer();
        uint32_t  buf[2];
        float *temp = (float *)buf;

        memcpy(buf,a,4);

        b[0] = temp[0];
       
        return(0);
    };


};

template<typename IN, int inputSize>
class TemperatureDisplay;

template<>
class TemperatureDisplay<float,1>: 
public GenericSink<float, 1>
{
public:
    TemperatureDisplay(FIFOBase<float> &src):
    GenericSink<float,1>(src){};

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final
    {
        float *b=this->getReadBuffer();
       
        
        printf("Temperature : %f\r\n",b[0]);
       
        return(0);
    };


};

template<typename IN1, int inputSize1,
         typename IN2, int inputSize2,
         typename IN3, int inputSize3,
         typename OUT1, int outputSize1,
         typename OUT2, int outputSize2,
         typename OUT3, int outputSize3>
class App;

template<typename IN1, int inputSize1,
         typename IN2, int inputSize2,
         typename IN3, int inputSize3>
class App<IN1,inputSize1,
          IN2,inputSize2,
          IN3,inputSize3,
          IN1,inputSize1,
          IN2,inputSize2,
          IN3,inputSize3>:public NodeBase
{
public:
     App(FIFOBase<IN1> &src1,
         FIFOBase<IN2> &src2,
         FIFOBase<IN3> &src3,
         FIFOBase<IN1> &dst1,
         FIFOBase<IN2> &dst2,
         FIFOBase<IN3> &dst3):
     mSrc1(src1),
     mSrc2(src2),
     mSrc3(src3),
     mDst1(dst1),
     mDst2(dst2),
     mDst3(dst3){
        canRun1=true;
        canRun2=true;
        canRun3=true;
     };

public:
    int prepareForRunning() final
    {
        if ((this->willUnderflow1()) || (this->willOverflow1()))
        {
            canRun1 = false;
        }
        else
        {
            canRun1=true;
        }

        if ((this->willUnderflow2()) || (this->willOverflow2()))
        {
            canRun2 = false;
        }
        else
        {
            canRun2=true;
        }

        if ((this->willUnderflow3()) || (this->willOverflow3()))
        {
            canRun3 = false;
        }
        else
        {
            canRun3=true;
        }

        if ((!canRun1) && (!canRun2) && (!canRun3))
        {
            return(CG_SKIP_EXECUTION_ID_CODE);
        }
        return(CG_SUCCESS);
    };

    int run() final
    {
        if (canRun1)
        {
           IN1 *in1=this->getReadBuffer1();
           IN1 *out1=this->getWriteBuffer1();

           memcpy(out1,in1,sizeof(IN1)*inputSize1);
        }

        if (canRun2)
        {
            IN2 *in2=this->getReadBuffer2();
            IN2 *out2=this->getWriteBuffer2();
        
            memcpy(out2,in2,sizeof(IN2)*inputSize2);
        }

        if (canRun3)
        {
            IN3 *in3=this->getReadBuffer3();
            IN3 *out3=this->getWriteBuffer3();
        
            memcpy(out3,in3,sizeof(IN3)*inputSize3);
        }

        return(CG_SUCCESS);
    };

protected:
     IN1 * getWriteBuffer1(int nb = inputSize1){return mDst1.getWriteBuffer(nb);};
     IN1 * getReadBuffer1(int nb = inputSize1){return mSrc1.getReadBuffer(nb);};

     bool willOverflow1(int nb = inputSize1){return mDst1.willOverflowWith(nb);};
     bool willUnderflow1(int nb = inputSize1){return mSrc1.willUnderflowWith(nb);};

     IN2 * getWriteBuffer2(int nb = inputSize2){return mDst2.getWriteBuffer(nb);};
     IN2 * getReadBuffer2(int nb = inputSize2){return mSrc2.getReadBuffer(nb);};

     bool willOverflow2(int nb = inputSize2){return mDst2.willOverflowWith(nb);};
     bool willUnderflow2(int nb = inputSize2){return mSrc2.willUnderflowWith(nb);};

     IN3 * getWriteBuffer3(int nb = inputSize3){return mDst3.getWriteBuffer(nb);};
     IN3 * getReadBuffer3(int nb = inputSize3){return mSrc3.getReadBuffer(nb);};

     bool willOverflow3(int nb = inputSize3){return mDst3.willOverflowWith(nb);};
     bool willUnderflow3(int nb = inputSize3){return mSrc3.willUnderflowWith(nb);};

     bool canRun1;
     bool canRun2;
     bool canRun3;

private:
    FIFOBase<IN1> &mSrc1;
    FIFOBase<IN2> &mSrc2;
    FIFOBase<IN3> &mSrc3;

    FIFOBase<IN1> &mDst1;
    FIFOBase<IN2> &mDst2;
    FIFOBase<IN3> &mDst3;
};

template<typename IN1, int inputSize1,
         typename IN2, int inputSize2,
         typename OUT1, int outputSize1,
         typename OUT2, int outputSize2>
class SmallApp;

template<typename IN1, int inputSize1,
         typename IN2, int inputSize2>
class SmallApp<IN1,inputSize1,
               IN2,inputSize2,
               IN1,inputSize1,
               IN2,inputSize2>:public NodeBase
{
public:
     SmallApp(FIFOBase<IN1> &src1,
              FIFOBase<IN2> &src2,
              FIFOBase<IN1> &dst1,
              FIFOBase<IN2> &dst2):
     mSrc1(src1),
     mSrc2(src2),
     mDst1(dst1),
     mDst2(dst2){
        canRun1=true;
        canRun2=true;
     };

public:
    int prepareForRunning() final
    {
        if ((this->willUnderflow1()) || (this->willOverflow1()))
        {
            canRun1 = false;
        }
        else
        {
            canRun1=true;
        }

        if ((this->willUnderflow2()) || (this->willOverflow2()))
        {
            canRun2 = false;
        }
        else
        {
            canRun2=true;
        }

       

        if ((!canRun1) && (!canRun2) )
        {
            return(CG_SKIP_EXECUTION_ID_CODE);
        }
        return(CG_SUCCESS);
    };

    int run() final
    {
        if (canRun1)
        {
           IN1 *in1=this->getReadBuffer1();
           IN1 *out1=this->getWriteBuffer1();

           memcpy(out1,in1,sizeof(IN1)*inputSize1);
        }

        if (canRun2)
        {
            IN2 *in2=this->getReadBuffer2();
            IN2 *out2=this->getWriteBuffer2();
        
            memcpy(out2,in2,sizeof(IN2)*inputSize2);
        }


        return(CG_SUCCESS);
    };

protected:
     IN1 * getWriteBuffer1(int nb = inputSize1){return mDst1.getWriteBuffer(nb);};
     IN1 * getReadBuffer1(int nb = inputSize1){return mSrc1.getReadBuffer(nb);};

     bool willOverflow1(int nb = inputSize1){return mDst1.willOverflowWith(nb);};
     bool willUnderflow1(int nb = inputSize1){return mSrc1.willUnderflowWith(nb);};

     IN2 * getWriteBuffer2(int nb = inputSize2){return mDst2.getWriteBuffer(nb);};
     IN2 * getReadBuffer2(int nb = inputSize2){return mSrc2.getReadBuffer(nb);};

     bool willOverflow2(int nb = inputSize2){return mDst2.willOverflowWith(nb);};
     bool willUnderflow2(int nb = inputSize2){return mSrc2.willUnderflowWith(nb);};

     bool canRun1;
     bool canRun2;

private:
    FIFOBase<IN1> &mSrc1;
    FIFOBase<IN2> &mSrc2;

    FIFOBase<IN1> &mDst1;
    FIFOBase<IN2> &mDst2;
};
#endif