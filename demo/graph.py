# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

# Import definitions used to describe the graph
from Stream.nodes import *
from Stream.sds_nodes import SDSSensor, SDSAsyncSensor,SDSRecorder

ASYNCHRONOUS = False 
RECORDER = True

ACC_BLOCK = 84

CANCEL_EVENT = 2
ACCELEROMETER_EVENT = 1

accelerometerType=CStructType("accelerometerSample_t",6)

if ASYNCHRONOUS:
    accelerometer = SDSAsyncSensor("accelerometer",ACC_BLOCK,
                              sds_yml_file="Recordings/Accelerometer.sds.yml",
                              sds_connection="demoContext->sensorConn_accelerometer")
else:
    accelerometer = SDSSensor("accelerometer",ACC_BLOCK,
                              sds_yml_file="Recordings/Accelerometer.sds.yml",
                              sds_connection="demoContext->sensorConn_accelerometer")

accelerometerRec = SDSRecorder("accelerometerRec",ACC_BLOCK,
                          sds_yml_file="Recordings/Accelerometer.sds.yml",
                          sds_connection="demoContext->recConn_accelerometer"
                          )

formatAccelerometer = FormatAccelerometer("format",
                                          accelerometerType,
                                          ACC_BLOCK,
                                          sample_size=accelerometer.sample_size)

accDisplay = AccelerometerDisplay("accDisplay",accelerometerType,ACC_BLOCK)

# Create a Graph object
the_graph = Graph()

the_graph.connect(accelerometer.o,formatAccelerometer.i)
the_graph.connect(formatAccelerometer.o,accDisplay.i)

if RECORDER:
    the_graph.connect(accelerometer.o,accelerometerRec.i)

header="""#ifndef _REC_CONFIG_H_
#define _REC_CONFIG_H_

%s

#endif
"""

with open("recorder_config.h","w") as f:
    r = "" 
    if RECORDER:
        r = "#define RECORDER_USED\n"
    print(header % r,file=f)

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

sched.ccode("./Stream",conf)

with open("Stream/demo.dot","w") as f:
    sched.graphviz(f,config=conf)
