import subprocess
# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

# Import definitions used to describe the graph
from Stream.nodes import *
from Stream.sds_nodes import SDSSensor,SDSRecorder

ACC_SENSOR=1 
GYRO_SENSOR=2 
TEMP_SENSOR=4
ALL_SENSORS=ACC_SENSOR|GYRO_SENSOR|TEMP_SENSOR
VEC_SENSORS=ACC_SENSOR|GYRO_SENSOR

# Use only ALL, VEC or TEMP

def sensor_mode_name(x):
    if x == ALL_SENSORS:
        return("all")
    if x == VEC_SENSORS:
        return("acc_gyro")
    if x == TEMP_SENSOR:
        return("temp")
    return("unknown")

def get_png_name(is_asynchronous,has_recorder,sensor_mode,timed):
    s="demo"
    if is_asynchronous:
        s+="_async"
    else:
        s+="_sync"
    if has_recorder:
        s+="_record"
    else:
        s+="_no_record"
    s+= f"_{sensor_mode_name(sensor_mode)}"
    if timed:
        s+="_timed"
    else:
        s+="_untimed"
    s+=".png"
    return(s)

def gen_png(is_asynchronous,has_recorder,sensor_mode,timed):
    output="GraphPictures/%s" % get_png_name(is_asynchronous=is_asynchronous,
                                             has_recorder=has_recorder,
                                             sensor_mode=sensor_mode,
                                             timed=timed) 
    result=subprocess.run(["dot","-Tpng","-o",output,"Stream/demo.dot"],timeout=600)


def gen_graph(is_asynchronous,
              has_recorder,
              sensor_mode=ALL_SENSORS,
              timed=False,
              fake_sensor = False,
              drift_delegate=False):

    if drift_delegate and fake_sensor:
        if timed:
           delegate="simple_timed_drift_correction"
           delegate_data="drift_data"
        else:
           delegate="simple_drift_correction"
           delegate_data="drift_data"
    else:
        delegate=None
        delegate_data=None
    ACC_TIMED_BLOCK = None 
    ACC_REC_TIMED_BLOCK = None

    TEMP_TIMED_BLOCK = None 
    TEMP_REC_TIMED_BLOCK = None
    if is_asynchronous:
       ACC_BLOCK = 100
       GYRO_BLOCK = 100
       if fake_sensor:
            TEMP_BLOCK = 50
       else:
            TEMP_BLOCK = 1
       if timed:
          ACC_TIMED_BLOCK = 100
          ACC_REC_TIMED_BLOCK = 100
          if fake_sensor:
             TEMP_TIMED_BLOCK = 50
             TEMP_REC_TIMED_BLOCK = 50
          else:
             TEMP_TIMED_BLOCK = 1
             TEMP_REC_TIMED_BLOCK = 1
    
       VEC_RECORD = 100
    else:
       if ((sensor_mode & ALL_SENSORS) == ALL_SENSORS):
          print("ALL SENSORS")
          ACC_BLOCK = 1667
          GYRO_BLOCK = 1667
          TEMP_BLOCK = 1

       
          VEC_RECORD = 1667

          if timed:
             ACC_TIMED_BLOCK = 1667
             ACC_REC_TIMED_BLOCK = 1667
             TEMP_TIMED_BLOCK = 1
             TEMP_REC_TIMED_BLOCK = 1
       elif ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
          print("VEC SENSORS")
          ACC_BLOCK = 100
          GYRO_BLOCK = 100
       
          VEC_RECORD = 10

          if timed:
             ACC_TIMED_BLOCK = 100
             ACC_REC_TIMED_BLOCK = 10
       elif ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
          print("TEMP SENSORS")
          if fake_sensor:
            TEMP_BLOCK = 50
  
            if timed:
               TEMP_TIMED_BLOCK = 50
               TEMP_REC_TIMED_BLOCK = 50
          else:
            TEMP_BLOCK = 1
  
            if timed:
               TEMP_TIMED_BLOCK = 1
               TEMP_REC_TIMED_BLOCK = 1
    
         
    
    
    CANCEL_EVENT = 2
    ACCELEROMETER_EVENT = 1

    if (sensor_mode == ALL_SENSORS):
        application = AppAll("application",ACC_BLOCK,GYRO_BLOCK,TEMP_BLOCK)
    elif (sensor_mode == VEC_SENSORS):
        application = AppVec("application",ACC_BLOCK,GYRO_BLOCK)
    else:
        application = AppTemp("application",TEMP_BLOCK)

    if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
        accelerometer = SDSSensor("accelerometerSensor",ACC_BLOCK,
                                  sds_yml_file="Recordings/Accelerometer.sds.yml",
                                  sds_connection="demoContext->sensorConn_accelerometer",
                                  timed=ACC_TIMED_BLOCK)
        gyroscope = SDSSensor("gyroscopeSensor",GYRO_BLOCK,
                                  sds_yml_file="Recordings/Gyroscope.sds.yml",
                                  sds_connection="demoContext->sensorConn_gyroscope")
    if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
        if fake_sensor:
            temperature = SDSSensor("fakeSensor",TEMP_BLOCK,
                                     sds_yml_file="Fake.sds.yml",
                                     sds_connection="demoContext->sensorConn_temperatureSensor",
                                     drift_delegate=delegate,
                                     drift_delegate_data=delegate_data,
                                     timed=TEMP_TIMED_BLOCK)
     
        else:
           temperature = SDSSensor("temperatureSensor",TEMP_BLOCK,
                                     sds_yml_file="Recordings/Temperature.sds.yml",
                                     sds_connection="demoContext->sensorConn_temperatureSensor",
                                     timed=TEMP_TIMED_BLOCK)
    
    if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
        accelerometerRec = SDSRecorder("accelerometerRecorder",CType(UINT8),VEC_RECORD,
                                  sds_yml_file="Recordings/Accelerometer.sds.yml",
                                  sds_connection="demoContext->recConn_accelerometer",
                                  timed=ACC_REC_TIMED_BLOCK
                                  )
        
        gyroscopeRec = SDSRecorder("gyroscopeRecorder",CType(UINT8),VEC_RECORD,
                                  sds_yml_file="Recordings/Gyroscope.sds.yml",
                                  sds_connection="demoContext->recConn_gyroscope"
                                  )
    
    if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
        if fake_sensor:
            temperatureRec = SDSRecorder("fakeRecorder",CType(UINT8),TEMP_BLOCK,
                                  sds_yml_file="Fake.sds.yml",
                                  sds_connection="demoContext->recConn_temperatureSensor",
                                  timed=TEMP_REC_TIMED_BLOCK
                                  )
    
        else:
            temperatureRec = SDSRecorder("temperatureRecorder",CType(UINT8),TEMP_BLOCK,
                                      sds_yml_file="Recordings/Temperature.sds.yml",
                                      sds_connection="demoContext->recConn_temperatureSensor",
                                      timed=TEMP_REC_TIMED_BLOCK
                                      )
    
    if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
       formatAccelerometer = FormatVector("formatAcc",ACC_BLOCK)
       accDisplay = VectorDisplay("accelerometer",ACC_BLOCK)
       
       formatGyroscope = FormatVector("formatGyro",GYRO_BLOCK)
       gyroDisplay = VectorDisplay("gyroscope",GYRO_BLOCK)
    
    if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
       if fake_sensor:
          formatTemperature = FormatTemperature("formatTemp",nb=TEMP_BLOCK)
          tempDisplay = TemperatureDisplay("temperature",nb=TEMP_BLOCK)

       else:
          formatTemperature = FormatTemperature("formatTemp")
          tempDisplay = TemperatureDisplay("temperature")
    
    # Create a Graph object
    the_graph = Graph()
    
    # Sensors and formatting
    if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
       the_graph.connect(accelerometer.o,formatAccelerometer.i)
       the_graph.connect(gyroscope.o,formatGyroscope.i)
    
    if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
       the_graph.connect(temperature.o,formatTemperature.i)
    
    # Application
    # To force the graph to be connected otherwise
    # we just have 3 separated graphs for each sensor / recorder
    if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
       the_graph.connect(formatAccelerometer.o,application.accI)
       the_graph.connect(formatGyroscope.o,application.gyroI)
    
    if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
       the_graph.connect(formatTemperature.o,application.tempI)
    
    # Display of sensors
    if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
       the_graph.connect(application.accO,accDisplay.i)
       the_graph.connect(application.gyroO,gyroDisplay.i)
    if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
       the_graph.connect(application.tempO,tempDisplay.i)
    
    
    # Recorders
    if has_recorder:
        if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
           the_graph.connect(accelerometer.o,accelerometerRec.i)
           if timed:
              the_graph.connect(accelerometer.t,accelerometerRec.t)
   
           the_graph.connect(gyroscope.o,gyroscopeRec.i)
        if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
           the_graph.connect(temperature.o,temperatureRec.i)
           if timed:
              the_graph.connect(temperature.t,temperatureRec.t)
    else:
        if timed:
            if ((sensor_mode & VEC_SENSORS) == VEC_SENSORS):
               da = DebugSink("da",CType(UINT32),ACC_REC_TIMED_BLOCK)
               the_graph.connect(accelerometer.t,da.i)

            if ((sensor_mode & TEMP_SENSOR) == TEMP_SENSOR):
               db = DebugSink("db",CType(UINT32),TEMP_REC_TIMED_BLOCK)
               the_graph.connect(temperature.t,db.i)

    header="""#ifndef _REC_CONFIG_H_
    #define _REC_CONFIG_H_
    
    #define ACC_SENSOR 1 
    #define GYRO_SENSOR 2 
    #define TEMP_SENSOR 4
    #define ALL_SENSORS (ACC_SENSOR|GYRO_SENSOR|TEMP_SENSOR)
    #define VEC_SENSORS (ACC_SENSOR|GYRO_SENSOR)
    
    %s
    %s
    %s
    %s
    %s
    %s
    #endif
    """
    
    with open("Stream/demo_config.h","w") as f:
        r = "" 
        if has_recorder:
            r = "#define RECORDER_USED 1\n"
        a = ""
        if is_asynchronous:
            a = "#define ASYNCHRONOUS 1\n"
        
        t = "#define SENSOR_MODE %s\n" % sensor_mode
        ti = ""
        if timed:
            ti = "#define TIMED %d\n" % timed
        fa = ""
        if fake_sensor:
            fa = "#define FAKE_SENSOR 1\n"
        de="" 
        if fake_sensor and drift_delegate:
            de="#define DRIFT_DELEGATE 1\n"
        print(header % (r,a,t,ti,fa,de),file=f)
    
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
            sensor_mode=sensor_mode,
            timed=timed)

import itertools
allConfigs = [
   [True,False],
   [True,False],
   [True,False],
   [ALL_SENSORS,VEC_SENSORS,TEMP_SENSOR]
]

GRID = False 
if GRID:
    for asyn,record,timed,mode in itertools.product(*allConfigs):
        print(f"async={asyn},has record={record}, timed={timed},mode={mode}")
        if timed:
           if not record:
              continue
        
        gen_graph(is_asynchronous=asyn,
                     has_recorder=record,
                     sensor_mode=mode,
                     timed=timed)

else:
    gen_graph(is_asynchronous=False,
              has_recorder=True,
              sensor_mode=TEMP_SENSOR,
              timed=True,
              fake_sensor=True,
              drift_delegate=True)

