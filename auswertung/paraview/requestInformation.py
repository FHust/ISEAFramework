import numpy as np
folder = "/tmp/"

def SetOutputTimesteps(algorithm, timesteps):
      executive = algorithm.GetExecutive()
      outInfo = executive.GetOutputInformation(0)
      outInfo.Remove(executive.TIME_STEPS())
      for timestep in timesteps:
        outInfo.Append(executive.TIME_STEPS(), timestep)
      outInfo.Remove(executive.TIME_RANGE())
      outInfo.Append(executive.TIME_RANGE(), timesteps[0])
      outInfo.Append(executive.TIME_RANGE(), timesteps[-1])

Time = np.loadtxt( folder + 'Init/Patch_Temperatures.csv', delimiter=',', comments='#', usecols=0)


try:
    SetOutputTimesteps(self, Time)
except:
    SetOutputTimesteps(self, (Time,))

