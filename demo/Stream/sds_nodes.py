from cmsis_stream.cg.scheduler import *
import yaml

class NumberOfSamplesMissingForIO(Exception):
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
    
    sensorType=CType(SINT8)

    return(sampleSize)

class SDSSource(GenericSource):
    def __init__(self,name,nb_samples,
        sds_yml_file=None,
        cmsis_rtos_event=None,
        cmsis_rtos_cancel_event=None,
        timeout="osWaitForever",
        sds_id = None):
        GenericSource.__init__(self,name)
        self.sample_size = getSensorSampleSize(sds_yml_file)
        self.addOutput("o",CType(SINT8),self.sample_size*nb_samples)
        
        if isinstance(cmsis_rtos_event, int):
           self.addLiteralArg(cmsis_rtos_event)
        else:
           self.addVariableArg(cmsis_rtos_event)

        if isinstance(cmsis_rtos_cancel_event, int):
           self.addLiteralArg(cmsis_rtos_cancel_event)
        else:
           self.addVariableArg(cmsis_rtos_cancel_event)

        if isinstance(timeout, int):
           self.addLiteralArg(timeout)
        else:
           self.addVariableArg(timeout)

        self.addVariableArg(sds_id)

    @property
    def typeName(self):
        return ("SDSSource")

class SDSAsyncSource(GenericSource):
    def __init__(self,name,nb_samples,
        sds_yml_file=None,
        cmsis_rtos_event=None,
        cmsis_rtos_cancel_event=None,
        timeout="osWaitForever",
        sds_id = None):
        GenericSource.__init__(self,name)
        self.sample_size = getSensorSampleSize(sds_yml_file)
        self.addOutput("o",CType(SINT8),self.sample_size*nb_samples)
        
        if isinstance(cmsis_rtos_cancel_event, int):
           self.addLiteralArg(cmsis_rtos_cancel_event)
        else:
           self.addVariableArg(cmsis_rtos_cancel_event)

        self.addVariableArg(sds_id)

    @property
    def typeName(self):
        return ("SDSAsyncSource")

class SDSRec(GenericSink):
    def __init__(self,name,nb_samples,
                 sds_yml_file=None,
                 sensor_name="Unknown",
                 rec_buffer=None,
                 rec_buffer_size=None,
                 rec_threshold=None):
        GenericSink.__init__(self,name)
        self.sample_size = getSensorSampleSize(sds_yml_file)
        self.addInput("i",CType(SINT8),self.sample_size*nb_samples)
            
        self.addLiteralArg(sensor_name)
        self.addVariableArg(rec_buffer)

        if isinstance(rec_buffer_size, int):
           self.addLiteralArg(rec_buffer_size)
        else:
           self.addVariableArg(rec_buffer_size)

        if isinstance(rec_threshold, int):
           self.addLiteralArg(rec_threshold)
        else:
           self.addVariableArg(rec_threshold)

    @property
    def typeName(self):
        return ("SDSRec")
