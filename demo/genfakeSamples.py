import struct
import numpy as np
import argparse

formatter = lambda prog: argparse.HelpFormatter(prog,max_help_position=60)
parser = argparse.ArgumentParser(description="Gen fake data",
                                 formatter_class=formatter)

parser.add_argument("-f", dest="sampling_freq", type=float,
                            help="Sampling frequency", 
                            default=100.0)


parser.add_argument("-d", dest="duration", type=float,
                            help="Duration", 
                            default=50.00)

parser.add_argument("-b", dest="nb_block_samples", type=int,
                            help="Block size", 
                            default=1)
    
args = parser.parse_args()

def getDataType(data_type):
    match data_type:
        case "int16_t":
            return "h"
        case "uint16_t":
            return "H"
        case "int32_t":
            return "i"
        case "uint32_t":
            return "I"
        case "float":
            return "f"
        case "double":
            return "d"
        case _:
            print(f"Unknown data type: {data_type}\n")
            return None

def signal(t):
    return(np.sin(np.pi*2.0*t))

NB_SAMPLES = int(args.duration * args.sampling_freq)
l=list(np.linspace(0,args.duration,NB_SAMPLES,dtype=float))

s = np.array_split(l, np.arange(args.nb_block_samples, len(l), args.nb_block_samples))

d_type = getDataType("float")

DELTA_TIME = 1.0*args.nb_block_samples/args.sampling_freq / 1.0e-3
timestamp = 0.0
with open("Fake.0.sds","wb") as of:
  for block in s:
      values = signal(block)
      if (len(values)<args.nb_block_samples):
        delta = args.nb_block_samples - len(values)
        values = np.pad(values, [(0, delta)], mode='constant')
      if (args.nb_block_samples!=len(values)):
         raise RuntimeError
      samples=struct.pack(f"II{int(len(values))}{d_type}", np.uint32(np.round(timestamp)),4*args.nb_block_samples,*values)
      of.write(samples)
      timestamp = timestamp + DELTA_TIME

