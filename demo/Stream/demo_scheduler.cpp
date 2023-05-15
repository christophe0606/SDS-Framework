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
static unsigned int schedule[5]=
{ 
1,3,2,4,0,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 84
#define FIFOSIZE1 504
#define FIFOSIZE2 504
#define FIFOSIZE3 504

#define BUFFERSIZE1 84
CG_BEFORE_BUFFER
accelerometerSample_t demo_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 504
CG_BEFORE_BUFFER
int8_t demo_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 504
CG_BEFORE_BUFFER
int8_t demo_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 504
CG_BEFORE_BUFFER
int8_t demo_buf4[BUFFERSIZE4]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t demo_scheduler(int *error,demoContext_t *demoContext)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<accelerometerSample_t,FIFOSIZE0,1,0> fifo0(demo_buf1);
    FIFO<int8_t,FIFOSIZE1,1,0> fifo1(demo_buf2);
    FIFO<int8_t,FIFOSIZE2,1,0> fifo2(demo_buf3);
    FIFO<int8_t,FIFOSIZE3,1,0> fifo3(demo_buf4);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    AccelerometerDisplay<accelerometerSample_t,84> accDisplay(fifo0);
    SDSSensor<int8_t,504> accelerometer(fifo1,demoContext->sensorConn_accelerometer);
    SDSRecorder<int8_t,504> accelerometerRec(fifo3,demoContext->recConn_accelerometer);
    Duplicate2<int8_t,504,int8_t,504,int8_t,504> dup0(fifo1,fifo2,fifo3);
    FormatAccelerometer<int8_t,504,accelerometerSample_t,84> format(fifo2,fifo0);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 5; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = accDisplay.run();
                }
                break;

                case 1:
                {
                   cgStaticError = accelerometer.run();
                }
                break;

                case 2:
                {
                   cgStaticError = accelerometerRec.run();
                }
                break;

                case 3:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 4:
                {
                   cgStaticError = format.run();
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
