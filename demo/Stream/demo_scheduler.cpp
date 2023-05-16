/*

Generated with CMSIS-DSP Compute Graph Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/


#include <cstdint>
#include "globalCGSettings.h"
#include "GenericNodes.h"
#include "DemoNodes.h"
#include "demo_scheduler.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION
#endif

CG_AFTER_INCLUDES


/*

Description of the scheduling. 

*/
static unsigned int schedule[29]=
{ 
2,4,1,1,1,1,1,1,1,1,1,1,6,10,5,9,9,9,9,9,9,9,9,9,9,7,3,0,8,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 100
#define FIFOSIZE1 100
#define FIFOSIZE2 100
#define FIFOSIZE3 100
#define FIFOSIZE4 600
#define FIFOSIZE5 600
#define FIFOSIZE6 600
#define FIFOSIZE7 600
#define FIFOSIZE8 600
#define FIFOSIZE9 600

#define BUFFERSIZE1 100
CG_BEFORE_BUFFER
vectorSample_t demo_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 100
CG_BEFORE_BUFFER
vectorSample_t demo_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 100
CG_BEFORE_BUFFER
vectorSample_t demo_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 100
CG_BEFORE_BUFFER
vectorSample_t demo_buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 600
CG_BEFORE_BUFFER
int8_t demo_buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 600
CG_BEFORE_BUFFER
int8_t demo_buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 600
CG_BEFORE_BUFFER
int8_t demo_buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 600
CG_BEFORE_BUFFER
int8_t demo_buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 600
CG_BEFORE_BUFFER
int8_t demo_buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 600
CG_BEFORE_BUFFER
int8_t demo_buf10[BUFFERSIZE10]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t demo_scheduler(int *error,demoContext_t *demoContext)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<vectorSample_t,FIFOSIZE0,1,0> fifo0(demo_buf1);
    FIFO<vectorSample_t,FIFOSIZE1,1,0> fifo1(demo_buf2);
    FIFO<vectorSample_t,FIFOSIZE2,1,0> fifo2(demo_buf3);
    FIFO<vectorSample_t,FIFOSIZE3,1,0> fifo3(demo_buf4);
    FIFO<int8_t,FIFOSIZE4,1,0> fifo4(demo_buf5);
    FIFO<int8_t,FIFOSIZE5,1,0> fifo5(demo_buf6);
    FIFO<int8_t,FIFOSIZE6,0,0> fifo6(demo_buf7);
    FIFO<int8_t,FIFOSIZE7,1,0> fifo7(demo_buf8);
    FIFO<int8_t,FIFOSIZE8,1,0> fifo8(demo_buf9);
    FIFO<int8_t,FIFOSIZE9,0,0> fifo9(demo_buf10);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    VectorDisplay<vectorSample_t,100> accelerometer(fifo2,"accelerometer");
    SDSRecorder<int8_t,60> accelerometerRecorder(fifo6,demoContext->recConn_accelerometer);
    SDSSensor<int8_t,600> accelerometerSensor(fifo4,demoContext->sensorConn_accelerometer);
    SmallApp<vectorSample_t,100,vectorSample_t,100,vectorSample_t,100,vectorSample_t,100> application(fifo0,fifo1,fifo2,fifo3);
    Duplicate2<int8_t,600,int8_t,600,int8_t,600> dup0(fifo4,fifo5,fifo6);
    Duplicate2<int8_t,600,int8_t,600,int8_t,600> dup1(fifo7,fifo8,fifo9);
    FormatVector<int8_t,600,vectorSample_t,100> formatAcc(fifo5,fifo0);
    FormatVector<int8_t,600,vectorSample_t,100> formatGyro(fifo8,fifo1);
    VectorDisplay<vectorSample_t,100> gyroscope(fifo3,"gyroscope");
    SDSRecorder<int8_t,60> gyroscopeRecorder(fifo9,demoContext->recConn_gyroscope);
    SDSSensor<int8_t,600> gyroscopeSensor(fifo7,demoContext->sensorConn_gyroscope);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 29; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = accelerometer.run();
                }
                break;

                case 1:
                {
                   cgStaticError = accelerometerRecorder.run();
                }
                break;

                case 2:
                {
                   cgStaticError = accelerometerSensor.run();
                }
                break;

                case 3:
                {
                   cgStaticError = application.run();
                }
                break;

                case 4:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 5:
                {
                   cgStaticError = dup1.run();
                }
                break;

                case 6:
                {
                   cgStaticError = formatAcc.run();
                }
                break;

                case 7:
                {
                   cgStaticError = formatGyro.run();
                }
                break;

                case 8:
                {
                   cgStaticError = gyroscope.run();
                }
                break;

                case 9:
                {
                   cgStaticError = gyroscopeRecorder.run();
                }
                break;

                case 10:
                {
                   cgStaticError = gyroscopeSensor.run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION;
            CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}
