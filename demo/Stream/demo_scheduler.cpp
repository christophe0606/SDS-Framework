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
static unsigned int schedule[10]=
{ 
1,3,7,4,9,5,2,0,6,8,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 10002
#define FIFOSIZE1 10002
#define FIFOSIZE2 4
#define FIFOSIZE3 1667
#define FIFOSIZE4 1667
#define FIFOSIZE5 1
#define FIFOSIZE6 1667
#define FIFOSIZE7 1667
#define FIFOSIZE8 1

#define BUFFERSIZE1 10002
CG_BEFORE_BUFFER
int8_t demo_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 10002
CG_BEFORE_BUFFER
int8_t demo_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 4
CG_BEFORE_BUFFER
int8_t demo_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 1
CG_BEFORE_BUFFER
float demo_buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 1
CG_BEFORE_BUFFER
float demo_buf9[BUFFERSIZE9]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t demo_scheduler(int *error,demoContext_t *demoContext)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<int8_t,FIFOSIZE0,1,0> fifo0(demo_buf1);
    FIFO<int8_t,FIFOSIZE1,1,0> fifo1(demo_buf2);
    FIFO<int8_t,FIFOSIZE2,1,0> fifo2(demo_buf3);
    FIFO<vectorSample_t,FIFOSIZE3,1,0> fifo3(demo_buf4);
    FIFO<vectorSample_t,FIFOSIZE4,1,0> fifo4(demo_buf5);
    FIFO<float,FIFOSIZE5,1,0> fifo5(demo_buf6);
    FIFO<vectorSample_t,FIFOSIZE6,1,0> fifo6(demo_buf7);
    FIFO<vectorSample_t,FIFOSIZE7,1,0> fifo7(demo_buf8);
    FIFO<float,FIFOSIZE8,1,0> fifo8(demo_buf9);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    VectorDisplay<vectorSample_t,1667> accelerometer(fifo6,"accelerometer");
    SDSSensor<int8_t,10002> accelerometerSensor(fifo0,demoContext->sensorConn_accelerometer);
    App<vectorSample_t,1667,vectorSample_t,1667,float,1,vectorSample_t,1667,vectorSample_t,1667,float,1> application(fifo3,fifo4,fifo5,fifo6,fifo7,fifo8);
    FormatVector<int8_t,10002,vectorSample_t,1667> formatAcc(fifo0,fifo3);
    FormatVector<int8_t,10002,vectorSample_t,1667> formatGyro(fifo1,fifo4);
    FormatTemperature<int8_t,4,float,1> formatTemp(fifo2,fifo5);
    VectorDisplay<vectorSample_t,1667> gyroscope(fifo7,"gyroscope");
    SDSSensor<int8_t,10002> gyroscopeSensor(fifo1,demoContext->sensorConn_gyroscope);
    TemperatureDisplay<float,1> temperature(fifo8);
    SDSSensor<int8_t,4> temperatureSensor(fifo2,demoContext->sensorConn_temperatureSensor);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 10; id++)
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
                   cgStaticError = accelerometerSensor.run();
                }
                break;

                case 2:
                {
                   cgStaticError = application.run();
                }
                break;

                case 3:
                {
                   cgStaticError = formatAcc.run();
                }
                break;

                case 4:
                {
                   cgStaticError = formatGyro.run();
                }
                break;

                case 5:
                {
                   cgStaticError = formatTemp.run();
                }
                break;

                case 6:
                {
                   cgStaticError = gyroscope.run();
                }
                break;

                case 7:
                {
                   cgStaticError = gyroscopeSensor.run();
                }
                break;

                case 8:
                {
                   cgStaticError = temperature.run();
                }
                break;

                case 9:
                {
                   cgStaticError = temperatureSensor.run();
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
