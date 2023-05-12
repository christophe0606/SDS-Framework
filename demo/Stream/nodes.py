from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource


class AccelerometerDisplay(GenericSink):
    def __init__(self,name,sampleType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",sampleType,inLength)

    @property
    def typeName(self):
        return "AccelerometerDisplay"

class TemperatureDisplay(GenericSink):
    def __init__(self,name,sampleType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",sampleType,inLength)

    @property
    def typeName(self):
        return "TemperatureDisplay"

class FormatAccelerometer(GenericNode):
    def __init__(self,name,sampleType,outLength,sample_size=1):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(SINT8),sample_size*outLength)
        self.addOutput("o",sampleType,outLength)

    @property
    def typeName(self):
        return "FormatAccelerometer"