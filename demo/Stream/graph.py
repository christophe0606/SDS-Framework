# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

# Import definitions used to describe the graph
from nodes import *
from sds_nodes import SDSSource, SDSAsyncSource,SDSRec

ASYNCHRONOUS = True 

ACC_BLOCK = 84

CANCEL_EVENT = 2
ACCELEROMETER_EVENT = 1

accelerometerType=CStructType("accelerometerSample_t",6)

if ASYNCHRONOUS:
    accelerometer = SDSAsyncSource("accelerometer",ACC_BLOCK,
                              sds_yml_file="../Recordings/Accelerometer.sds.yml",
                              cmsis_rtos_cancel_event = CANCEL_EVENT,
                              sds_id="demoContext->accId")
else:
    accelerometer = SDSSource("accelerometer",ACC_BLOCK,
                              sds_yml_file="../Recordings/Accelerometer.sds.yml",
                              cmsis_rtos_event = ACCELEROMETER_EVENT,
                              cmsis_rtos_cancel_event = CANCEL_EVENT,
                              sds_id="demoContext->accId")

accelerometerRec = SDSRec("accelerometerRec",ACC_BLOCK,
                          sds_yml_file="../Recordings/Accelerometer.sds.yml",
                          sensor_name="Accelerometer",
                          rec_buffer="demoContext->recBuf_accelerometer",
                          rec_buffer_size="demoContext->recBufSize_accelerometer",
                          rec_threshold="demoContext->recorderAccThreshold"
                          )

formatAccelerometer = FormatAccelerometer("format",
                                          accelerometerType,
                                          ACC_BLOCK,
                                          sample_size=accelerometer.sample_size)

accDisplay = AccelerometerDisplay("accDisplay",accelerometerType,ACC_BLOCK)

# Create a Graph object
the_graph = Graph()

the_graph.connect(accelerometer.o,formatAccelerometer.i)
the_graph.connect(accelerometer.o,accelerometerRec.i)

the_graph.connect(formatAccelerometer.o,accDisplay.i)


##############################
#
# Generation of the code and 
# graphviz representation
#
print("Generate graphviz and code")

conf=Configuration()

# Argument of the scheduler function.
# Variables used in the initialization of some nodes
conf.cOptionalArgs=["demoContext_t *demoContext"]

# Name of files included by the scheduler
conf.appNodesCName="DemoNodes.h"
conf.customCName="globalCGSettings.h"
conf.schedulerCFileName="demo_scheduler"
# Prefix for FIFO buffers
conf.prefix="demo_"
# Name of scheduler function
conf.schedName="demo_scheduler"

conf.asynchronous = ASYNCHRONOUS

# Enable event recorder tracing in the scheduler
#conf.eventRecorder=True

#conf.dumpSchedule  = True
#conf.displayFIFOBuf = True

sched = the_graph.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode(".",conf)

with open("demo.dot","w") as f:
    sched.graphviz(f,config=conf)
