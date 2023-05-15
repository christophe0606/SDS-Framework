from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource

class App(GenericNode):
    def __init__(self,name,accLength,gyroLength,tempLengh):
        GenericNode.__init__(self,name)
        vectorType=CStructType("vectorSample_t",6)
        self.addInput("accI",vectorType,accLength)
        self.addInput("gyroI",vectorType,gyroLength)
        self.addInput("tempI",CType(F32),tempLengh)

        self.addOutput("accO",vectorType,accLength)
        self.addOutput("gyroO",vectorType,gyroLength)
        self.addOutput("tempO",CType(F32),tempLengh)

    @property
    def typeName(self):
        return "App"

class VectorDisplay(GenericSink):
    def __init__(self,name,inLength):
        GenericSink.__init__(self,name)
        vectorType=CStructType("vectorSample_t",6)
        self.addInput("i",vectorType,inLength)
        self.addLiteralArg(name)

    @property
    def typeName(self):
        return "VectorDisplay"

class TemperatureDisplay(GenericSink):
    def __init__(self,name):
        GenericSink.__init__(self,name)
        self.addInput("i",CType(F32),1)

    @property
    def typeName(self):
        return "TemperatureDisplay"

class FormatTemperature(GenericNode):
    def __init__(self,name):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(SINT8),4)
        self.addOutput("o",CType(F32),1)

    @property
    def typeName(self):
        return "FormatTemperature"

class FormatVector(GenericNode):
    def __init__(self,name,outLength):
        GenericNode.__init__(self,name)
        vectorType=CStructType("vectorSample_t",6)

        self.addInput("i",CType(SINT8),6*outLength)
        self.addOutput("o",vectorType,outLength)

    @property
    def typeName(self):
        return "FormatVector"