import struct
import numpy as np
import argparse

formatter = lambda prog: argparse.HelpFormatter(prog,max_help_position=60)
parser = argparse.ArgumentParser(description="Gen fake data",
                                 formatter_class=formatter)

parser.add_argument("-f", dest="sampling_freq", type=float,
                            help="Sampling frequency", 
                            default=30.0)

# It is the clock of the sensor
# SDS using the clock of the AVH to generate
# the recorder sample
# we need to save the sample with a corrected
# timestamp to reproduce the problem.
parser.add_argument("-s", dest="sensor_sampling_freq", type=float,
                            help="Sampling frequency", 
                            default=31.0)

parser.add_argument("-d", dest="duration", type=float,
                            help="Duration", 
                            default=1.00)

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

freq_correction_factor = args.sampling_freq / args.sensor_sampling_freq

NB_SAMPLES = int(args.duration * args.sampling_freq)
l=list(np.linspace(0,args.duration/freq_correction_factor,NB_SAMPLES,dtype=float))

s = np.array_split(l, np.arange(args.nb_block_samples, len(l), args.nb_block_samples))

d_type = getDataType("float")

DELTA_TIME = np.uint32(1.0*args.nb_block_samples/args.sampling_freq / 1.0e-3)
timestamp = 0
with open("Fake.0.sds","wb") as of:
  for block in s:
      values = signal(block)
      if (len(values)<args.nb_block_samples):
        delta = args.nb_block_samples - len(values)
        values = np.pad(values, [(0, delta)], mode='constant')
      if (args.nb_block_samples!=len(values)):
         raise RuntimeError
      corrected_timestamp = int(timestamp * freq_correction_factor)
      samples=struct.pack(f"II{int(len(values))}{d_type}", corrected_timestamp,4*args.nb_block_samples,*values)
      of.write(samples)
      timestamp = timestamp + DELTA_TIME# 10 ms

