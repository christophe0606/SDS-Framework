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
2,1,3,0,4,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 200
#define FIFOSIZE1 50
#define FIFOSIZE2 50
#define FIFOSIZE3 50

#define BUFFERSIZE1 200
CG_BEFORE_BUFFER
uint8_t demo_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 50
CG_BEFORE_BUFFER
float demo_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 50
CG_BEFORE_BUFFER
float demo_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 50
CG_BEFORE_BUFFER
uint32_t demo_buf4[BUFFERSIZE4]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t demo_scheduler(int *error,demoContext_t *demoContext)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<uint8_t,FIFOSIZE0,0,1> fifo0(demo_buf1);
    FIFO<float,FIFOSIZE1,0,1> fifo1(demo_buf2);
    FIFO<float,FIFOSIZE2,0,1> fifo2(demo_buf3);
    FIFO<uint32_t,FIFOSIZE3,0,1> fifo3(demo_buf4);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    AppTemp<float,50,float,50> application(fifo1,fifo2);
    DebugSink<uint32_t,50> db(fifo3);
    SDSTimedSensor<uint8_t,200,uint32_t,50> fakeSensor(fifo0,fifo3,demoContext->sensorConn_temperatureSensor);
    FormatTemperature<uint8_t,200,float,50> formatTemp(fifo0,fifo1);
    TemperatureDisplay<float,50> temperature(fifo2);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 5; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            cgStaticError = 0;
            switch(schedule[id])
            {
                case 0:
                {
                    cgStaticError = application.prepareForRunning();
                }
                break;

                case 1:
                {
                    cgStaticError = db.prepareForRunning();
                }
                break;

                case 2:
                {
                    cgStaticError = fakeSensor.prepareForRunning();
                }
                break;

                case 3:
                {
                    cgStaticError = formatTemp.prepareForRunning();
                }
                break;

                case 4:
                {
                    cgStaticError = temperature.prepareForRunning();
                }
                break;

                default:
                break;
            }

            if (cgStaticError == CG_SKIP_EXECUTION_ID_CODE)
            { 
              cgStaticError = 0;
              continue;
            }

            CHECKERROR;

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = application.run();
                }
                break;

                case 1:
                {
                   cgStaticError = db.run();
                }
                break;

                case 2:
                {
                   cgStaticError = fakeSensor.run();
                }
                break;

                case 3:
                {
                   cgStaticError = formatTemp.run();
                }
                break;

                case 4:
                {
                   cgStaticError = temperature.run();
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
