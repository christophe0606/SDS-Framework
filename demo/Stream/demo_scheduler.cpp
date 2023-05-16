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
static unsigned int schedule[16]=
{ 
2,4,1,7,12,5,11,8,15,6,14,9,3,0,10,13,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 1667
#define FIFOSIZE1 1667
#define FIFOSIZE2 1
#define FIFOSIZE3 1667
#define FIFOSIZE4 1667
#define FIFOSIZE5 1
#define FIFOSIZE6 10002
#define FIFOSIZE7 10002
#define FIFOSIZE8 10002
#define FIFOSIZE9 10002
#define FIFOSIZE10 10002
#define FIFOSIZE11 10002
#define FIFOSIZE12 4
#define FIFOSIZE13 4
#define FIFOSIZE14 4

#define BUFFERSIZE1 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 1
CG_BEFORE_BUFFER
float demo_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 1667
CG_BEFORE_BUFFER
vectorSample_t demo_buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 1
CG_BEFORE_BUFFER
float demo_buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 10002
CG_BEFORE_BUFFER
int8_t demo_buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 10002
CG_BEFORE_BUFFER
int8_t demo_buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 10002
CG_BEFORE_BUFFER
int8_t demo_buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 10002
CG_BEFORE_BUFFER
int8_t demo_buf10[BUFFERSIZE10]={0};

#define BUFFERSIZE11 10002
CG_BEFORE_BUFFER
int8_t demo_buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 10002
CG_BEFORE_BUFFER
int8_t demo_buf12[BUFFERSIZE12]={0};

#define BUFFERSIZE13 4
CG_BEFORE_BUFFER
int8_t demo_buf13[BUFFERSIZE13]={0};

#define BUFFERSIZE14 4
CG_BEFORE_BUFFER
int8_t demo_buf14[BUFFERSIZE14]={0};

#define BUFFERSIZE15 4
CG_BEFORE_BUFFER
int8_t demo_buf15[BUFFERSIZE15]={0};


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
    FIFO<float,FIFOSIZE2,1,0> fifo2(demo_buf3);
    FIFO<vectorSample_t,FIFOSIZE3,1,0> fifo3(demo_buf4);
    FIFO<vectorSample_t,FIFOSIZE4,1,0> fifo4(demo_buf5);
    FIFO<float,FIFOSIZE5,1,0> fifo5(demo_buf6);
    FIFO<int8_t,FIFOSIZE6,1,0> fifo6(demo_buf7);
    FIFO<int8_t,FIFOSIZE7,1,0> fifo7(demo_buf8);
    FIFO<int8_t,FIFOSIZE8,1,0> fifo8(demo_buf9);
    FIFO<int8_t,FIFOSIZE9,1,0> fifo9(demo_buf10);
    FIFO<int8_t,FIFOSIZE10,1,0> fifo10(demo_buf11);
    FIFO<int8_t,FIFOSIZE11,1,0> fifo11(demo_buf12);
    FIFO<int8_t,FIFOSIZE12,1,0> fifo12(demo_buf13);
    FIFO<int8_t,FIFOSIZE13,1,0> fifo13(demo_buf14);
    FIFO<int8_t,FIFOSIZE14,1,0> fifo14(demo_buf15);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    VectorDisplay<vectorSample_t,1667> accelerometer(fifo3,"accelerometer");
    SDSRecorder<int8_t,10002> accelerometerRecorder(fifo8,demoContext->recConn_accelerometer);
    SDSSensor<int8_t,10002> accelerometerSensor(fifo6,demoContext->sensorConn_accelerometer);
    App<vectorSample_t,1667,vectorSample_t,1667,float,1,vectorSample_t,1667,vectorSample_t,1667,float,1> application(fifo0,fifo1,fifo2,fifo3,fifo4,fifo5);
    Duplicate2<int8_t,10002,int8_t,10002,int8_t,10002> dup0(fifo6,fifo7,fifo8);
    Duplicate2<int8_t,10002,int8_t,10002,int8_t,10002> dup1(fifo9,fifo10,fifo11);
    Duplicate2<int8_t,4,int8_t,4,int8_t,4> dup2(fifo12,fifo13,fifo14);
    FormatVector<int8_t,10002,vectorSample_t,1667> formatAcc(fifo7,fifo0);
    FormatVector<int8_t,10002,vectorSample_t,1667> formatGyro(fifo10,fifo1);
    FormatTemperature<int8_t,4,float,1> formatTemp(fifo13,fifo2);
    VectorDisplay<vectorSample_t,1667> gyroscope(fifo4,"gyroscope");
    SDSRecorder<int8_t,10002> gyroscopeRecorder(fifo11,demoContext->recConn_gyroscope);
    SDSSensor<int8_t,10002> gyroscopeSensor(fifo9,demoContext->sensorConn_gyroscope);
    TemperatureDisplay<float,1> temperature(fifo5);
    SDSRecorder<int8_t,4> temperatureRecorder(fifo14,demoContext->recConn_temperatureSensor);
    SDSSensor<int8_t,4> temperatureSensor(fifo12,demoContext->sensorConn_temperatureSensor);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 16; id++)
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
                   cgStaticError = dup2.run();
                }
                break;

                case 7:
                {
                   cgStaticError = formatAcc.run();
                }
                break;

                case 8:
                {
                   cgStaticError = formatGyro.run();
                }
                break;

                case 9:
                {
                   cgStaticError = formatTemp.run();
                }
                break;

                case 10:
                {
                   cgStaticError = gyroscope.run();
                }
                break;

                case 11:
                {
                   cgStaticError = gyroscopeRecorder.run();
                }
                break;

                case 12:
                {
                   cgStaticError = gyroscopeSensor.run();
                }
                break;

                case 13:
                {
                   cgStaticError = temperature.run();
                }
                break;

                case 14:
                {
                   cgStaticError = temperatureRecorder.run();
                }
                break;

                case 15:
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
