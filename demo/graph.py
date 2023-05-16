import subprocess
# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

# Import definitions used to describe the graph
from Stream.nodes import *
from Stream.sds_nodes import SDSSensor,SDSRecorder


def get_png_name(is_asynchronous,has_recorder,has_temp):
    s="demo"
    if is_asynchronous:
        s+="_async"
    else:
        s+="_sync"
    if has_recorder:
        s+="_record"
    else:
        s+="_no_record"
    if has_temp:
        s+="_temp"
    else:
        s+="_no_temp"
    s+=".png"
    return(s)

def gen_png(is_asynchronous,has_recorder,has_temp):
    output="GraphPictures/%s" % get_png_name(is_asynchronous=is_asynchronous,
                                             has_recorder=has_recorder,
                                             has_temp=has_temp) 
    result=subprocess.run(["dot","-Tpng","-o",output,"Stream/demo.dot"],timeout=600)


def gen_graph(is_asynchronous,has_recorder,has_temp):
    if is_asynchronous:
       ACC_BLOCK = 100
       GYRO_BLOCK = 100
       TEMP_BLOCK = 1
    
       VEC_RECORD = 100
    else:
       if not has_temp:
          ACC_BLOCK = 100
          GYRO_BLOCK = 100
       
          VEC_RECORD = 10
       else:
          ACC_BLOCK = 1667
          GYRO_BLOCK = 1667
          TEMP_BLOCK = 1
       
          VEC_RECORD = 1667
    
    CANCEL_EVENT = 2
    ACCELEROMETER_EVENT = 1
    
    if not has_temp:
        application = SmallApp("application",ACC_BLOCK,GYRO_BLOCK)
    else:
        application = App("application",ACC_BLOCK,GYRO_BLOCK,TEMP_BLOCK)
    
    accelerometer = SDSSensor("accelerometerSensor",ACC_BLOCK,
                              sds_yml_file="Recordings/Accelerometer.sds.yml",
                              sds_connection="demoContext->sensorConn_accelerometer",
                              asynchronous=is_asynchronous)
    gyroscope = SDSSensor("gyroscopeSensor",GYRO_BLOCK,
                              sds_yml_file="Recordings/Gyroscope.sds.yml",
                              sds_connection="demoContext->sensorConn_gyroscope",
                              asynchronous=is_asynchronous)
    
    if has_temp:
       temperature = SDSSensor("temperatureSensor",TEMP_BLOCK,
                                 sds_yml_file="Recordings/Temperature.sds.yml",
                                 sds_connection="demoContext->sensorConn_temperatureSensor",
                                 asynchronous=is_asynchronous)
    
    accelerometerRec = SDSRecorder("accelerometerRecorder",VEC_RECORD,
                              sds_yml_file="Recordings/Accelerometer.sds.yml",
                              sds_connection="demoContext->recConn_accelerometer"
                              )
    
    gyroscopeRec = SDSRecorder("gyroscopeRecorder",VEC_RECORD,
                              sds_yml_file="Recordings/Gyroscope.sds.yml",
                              sds_connection="demoContext->recConn_gyroscope"
                              )
    
    if has_temp:
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
    
    if has_temp:
       the_graph.connect(temperature.o,formatTemperature.i)
    
    # To force the graph to be connected otherwise
    # we just have 3 separated graphs for each sensor / recorder
    the_graph.connect(formatAccelerometer.o,application.accI)
    the_graph.connect(formatGyroscope.o,application.gyroI)
    if has_temp:
       the_graph.connect(formatTemperature.o,application.tempI)
    
    the_graph.connect(application.accO,accDisplay.i)
    the_graph.connect(application.gyroO,gyroDisplay.i)
    if has_temp:
       the_graph.connect(application.tempO,tempDisplay.i)
    
    
    if has_recorder:
        the_graph.connect(accelerometer.o,accelerometerRec.i)
        the_graph.connect(gyroscope.o,gyroscopeRec.i)
        if has_temp:
           the_graph.connect(temperature.o,temperatureRec.i)
    
    header="""#ifndef _REC_CONFIG_H_
    #define _REC_CONFIG_H_
    
    %s
    %s
    %s
    #endif
    """
    
    with open("demo_config.h","w") as f:
        r = "" 
        if has_recorder:
            r = "#define RECORDER_USED\n"
        a = ""
        if is_asynchronous:
            a = "#define ASYNCHRONOUS\n"
        t = ""
        if has_temp:
            t = "#define TEMPSENSOR\n"
        print(header % (r,a,t),file=f)
    
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
    
    conf.asynchronous = is_asynchronous
    
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
    
    gen_png(is_asynchronous=is_asynchronous,
            has_recorder=has_recorder,
            has_temp=has_temp)

import itertools
allConfigs = [
   [True,False],
   [True,False],
   [True,False]
]
#for a,b,c in itertools.product(*allConfigs):
#    gen_graph(is_asynchronous=a,
#              has_recorder=b,
#              has_temp=c)

gen_graph(is_asynchronous=False,
          has_recorder=True,
          has_temp=True)