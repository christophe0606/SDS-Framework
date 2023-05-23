from cmsis_stream.cg.scheduler import *
from cmsis_stream.cg.scheduler import GenericNode,GenericSink,GenericSource

class AppAll(GenericNode):
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
        return "AppAll"

class AppVec(GenericNode):
    def __init__(self,name,accLength,gyroLength):
        GenericNode.__init__(self,name)
        vectorType=CStructType("vectorSample_t",6)
        self.addInput("accI",vectorType,accLength)
        self.addInput("gyroI",vectorType,gyroLength)

        self.addOutput("accO",vectorType,accLength)
        self.addOutput("gyroO",vectorType,gyroLength)

    @property
    def typeName(self):
        return "AppVec"

class AppTemp(GenericNode):
    def __init__(self,name,tempLengh):
        GenericNode.__init__(self,name)
        self.addInput("tempI",CType(F32),tempLengh)

        self.addOutput("tempO",CType(F32),tempLengh)


    @property
    def typeName(self):
        return "AppTemp"

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
    def __init__(self,name,nb=1):
        GenericSink.__init__(self,name)
        self.addInput("i",CType(F32),nb)

    @property
    def typeName(self):
        return "TemperatureDisplay"

class FormatTemperature(GenericNode):
    def __init__(self,name,nb=1):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(UINT8),4*nb)
        self.addOutput("o",CType(F32),nb)

    @property
    def typeName(self):
        return "FormatTemperature"

class FormatVector(GenericNode):
    def __init__(self,name,outLength):
        GenericNode.__init__(self,name)
        vectorType=CStructType("vectorSample_t",6)

        self.addInput("i",CType(UINT8),6*outLength)
        self.addOutput("o",vectorType,outLength)

    @property
    def typeName(self):
        return "FormatVector"

class DebugNode(GenericNode):
    def __init__(self,name,ti,to,inLength,outLength):
        GenericNode.__init__(self,name)

        self.addInput("i",ti,inLength)
        self.addOutput("o",to,outLength)

    @property
    def typeName(self):
        return "DebugNode"

class DebugSource(GenericSource):
    def __init__(self,name,t,nb_samples,
                 ):
        GenericSink.__init__(self,name)
        
        self.addOutput("o",t,nb_samples)
    

    @property
    def typeName(self):
        return ("DebugSource")
        
class DebugSink(GenericSink):
    def __init__(self,name,t,nb_samples,
                 ):
        GenericSink.__init__(self,name)
        
        self.addInput("i",t,nb_samples)
    

    @property
    def typeName(self):
        return ("DebugSink")