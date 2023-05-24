from cmsis_stream.cg.scheduler import *
import yaml

class TimestampPeriodNotDivisorOfSamplePeriod(Exception):
    pass

class DelegateFunctionAndDelegateDataNeeded(Exception):
    pass

# Open SDS data file in read mode
def openFile(file_name):
    try:
        if ".yml" in file_name:
            f = open(file_name, "r")
        else:
            f = open(file_name, "rb")

        return f
    except Exception as e:
        print(f"Error in openFile({file_name}): {e}")
        sys.exit(1)

# Close file
def closeFile(file_name):
    try:
        file_name.close()
    except Exception as e:
        print(f"Error in closeFile({file_name}): {e}")
        sys.exit(1)

def getDataTypeSizeInBytes(data_type):
    match data_type:
        case "int8_t":
            return 1
        case "uint8_t":
            return 1
        case "int16_t":
            return 2
        case "uint16_t":
            return 2
        case "int32_t":
            return 4
        case "uint32_t":
            return 4
        case "float":
            return 4
        case "double":
            return 8
        case _:
            print(f"Unknown data type: {data_type}\n")
            return None

def getSensorSampleSize(file_name):
    if ".yml" in file_name:
        with open(file_name, "r") as meta_file:
             meta_data = yaml.load(meta_file, Loader=yaml.FullLoader)["sds"]
    else: 
        with open(file_name, "rb") as meta_file:
             meta_data = yaml.load(meta_file, Loader=yaml.FullLoader)["sds"]
    data_name = meta_data["name"]
    struct_name = data_name + "Type"
    data_desc = meta_data["content"]
    data_freq = meta_data["frequency"]
    sampleSize = 0
    dt = []
    for c in meta_data["content"]:
        entryDataType = c['type'] 
        dt.append(entryDataType)
        sampleSize += getDataTypeSizeInBytes(entryDataType)
    
    sensorType=CType(UINT8)

    return(sampleSize)

class SDSSensor(GenericSource):
    def __init__(self,name,nb_samples,
        sds_yml_file=None,
        sds_connection=None,
        asynchronous=False,
        timed=None,
        drift_delegate=None,
        drift_delegate_data=None):
        GenericSource.__init__(self,name)
        self._timed=timed
        self.sample_size = getSensorSampleSize(sds_yml_file)
        self.addOutput("o",CType(UINT8),self.sample_size*nb_samples)
        if timed:
            if (nb_samples % timed) != 0:
                raise TimestampPeriodNotDivisorOfSamplePeriod
            self.addOutput("t",CType(UINT32),timed)

        self.addVariableArg(sds_connection)
        self._asynchronous = asynchronous

        if drift_delegate and drift_delegate_data:
           self.addVariableArg(drift_delegate)
           self.addVariableArg(drift_delegate_data)
        else:
            raise DelegateFunctionAndDelegateDataNeeded


    @property
    def typeName(self):
        if self._asynchronous:
           if self._timed:
              return ("SDSAsyncTimedSensor")
           else:
              return ("SDSAsyncSensor")
        else:
           if self._timed:
              return ("SDSTimedSensor")
           else:
              return ("SDSSensor")

class SDSRecorder(GenericSink):
    def __init__(self,name,the_type,nb_samples,
                 sds_yml_file=None,
                 sds_connection=None,
                 timed=None):
        GenericSink.__init__(self,name)
        self.sample_size = getSensorSampleSize(sds_yml_file)
        self.addInput("i",the_type,self.sample_size*nb_samples)
        self._timed = timed
        if timed:
           if (nb_samples % timed) != 0:
                raise TimestampPeriodNotDivisorOfSamplePeriod
           self.addInput("t",CType(UINT32),timed)

        self.addVariableArg(sds_connection)

    @property
    def typeName(self):
        if self._timed:
            return ("SDSTimedRecorder")
        else:
            return ("SDSRecorder")
