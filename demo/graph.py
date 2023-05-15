# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

# Import definitions used to describe the graph
from Stream.nodes import *
from Stream.sds_nodes import SDSSensor,SDSRecorder

ASYNCHRONOUS = False 
RECORDER = True

ACC_BLOCK = 1667
GYRO_BLOCK = 1667
TEMP_BLOCK = 1

CANCEL_EVENT = 2
ACCELEROMETER_EVENT = 1

application = App("application",ACC_BLOCK,GYRO_BLOCK,TEMP_BLOCK)

accelerometer = SDSSensor("accelerometerSensor",ACC_BLOCK,
                          sds_yml_file="Recordings/Accelerometer.sds.yml",
                          sds_connection="demoContext->sensorConn_accelerometer",
                          asynchronous=ASYNCHRONOUS)
gyroscope = SDSSensor("gyroscopeSensor",GYRO_BLOCK,
                          sds_yml_file="Recordings/Gyroscope.sds.yml",
                          sds_connection="demoContext->sensorConn_gyroscope",
                          asynchronous=ASYNCHRONOUS)
temperature = SDSSensor("temperatureSensor",TEMP_BLOCK,
                          sds_yml_file="Recordings/Temperature.sds.yml",
                          sds_connection="demoContext->sensorConn_temperatureSensor",
                          asynchronous=ASYNCHRONOUS)

accelerometerRec = SDSRecorder("accelerometerRecorder",ACC_BLOCK,
                          sds_yml_file="Recordings/Accelerometer.sds.yml",
                          sds_connection="demoContext->recConn_accelerometer"
                          )

gyroscopeRec = SDSRecorder("gyroscopeRecorder",GYRO_BLOCK,
                          sds_yml_file="Recordings/Gyroscope.sds.yml",
                          sds_connection="demoContext->recConn_gyroscope"
                          )

temperatureRec = SDSRecorder("temperatureRecorder",TEMP_BLOCK,
                          sds_yml_file="Recordings/Temperature.sds.yml",
                          sds_connection="demoContext->recConn_temperatureSensor"
                          )


formatAccelerometer = FormatVector("formatAcc",ACC_BLOCK)
accDisplay = VectorDisplay("accelerometer",ACC_BLOCK)

formatGyroscope = FormatVector("formatGyro",GYRO_BLOCK)
gyroDisplay = VectorDisplay("gyroscope",GYRO_BLOCK)

formatTemperature = FormatTemperature("formatTemp")
tempDisplay = TemperatureDisplay("temperature")

# Create a Graph object
the_graph = Graph()

the_graph.connect(accelerometer.o,formatAccelerometer.i)
the_graph.connect(gyroscope.o,formatGyroscope.i)
the_graph.connect(temperature.o,formatTemperature.i)

# To force the graph to be connected otherwise
# we just have 3 separated graphs for each sensor / recorder
the_graph.connect(formatAccelerometer.o,application.accI)
the_graph.connect(formatGyroscope.o,application.gyroI)
the_graph.connect(formatTemperature.o,application.tempI)

the_graph.connect(application.accO,accDisplay.i)
the_graph.connect(application.gyroO,gyroDisplay.i)
the_graph.connect(application.tempO,tempDisplay.i)


if RECORDER:
    the_graph.connect(accelerometer.o,accelerometerRec.i)
    the_graph.connect(gyroscope.o,gyroscopeRec.i)
    the_graph.connect(temperature.o,temperatureRec.i)

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
