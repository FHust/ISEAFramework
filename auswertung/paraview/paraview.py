#!/usr/bin/env python
# -*- coding: utf-8 -*-

import vtk
from vtk.numpy_interface import algorithms as algs
from vtk.numpy_interface import dataset_adapter as dsa
import numpy as np

folder = "/tmp/"


def GetUpdateTimestep(algorithm):
      """Returns the requested time value, or None if not present"""
      executive = algorithm.GetExecutive()
      outInfo = executive.GetOutputInformation(0)
      if not outInfo.Has(executive.UPDATE_TIME_STEP()):
          return None
      return outInfo.Get(executive.UPDATE_TIME_STEP())
  # This is the requested time-step. This may not be exactly equal to the
  # timesteps published in RequestInformation(). Your code must handle that
  # correctly
req_time = GetUpdateTimestep(self)

output = self.GetOutput()



def readFile(name, dtype = np.float64):
    returnValue = []
    with open(name, 'r') as f:
        for line in f:
            returnValue.append( np.fromstring(line, dtype=dtype, sep=','))
    return returnValue

def mkVtkIdList(it):
    vil = vtk.vtkIdList()
    for i in it:
        vil.InsertNextId(int(i) -1)
    return vil



areas = readFile(folder + 'Init/Patch_Areas.csv', dtype=np.int64)
Temperatures = np.loadtxt( folder + 'Init/Patch_Temperatures.csv', delimiter=',', comments='#')
vertices = readFile(folder + 'Init/Patch_Vertices.csv')

T_min = np.min(Temperatures) -2 
T_max = np.max(Temperatures) + 2
T_Delta = T_max -T_min
#print(Temperatures)
counter = 0
if (len(Temperatures.shape) ) ==1:
     Temperatures=np.resize(Temperatures, (1,Temperatures.shape[0]))


while req_time < Temperatures[counter][0]:
    counter += 1

index = counter
#print(Time)
##Data structure
points  = vtk.vtkPoints()
polys   = vtk.vtkCellArray()
scalars = vtk.vtkFloatArray()
scalars.SetName("Temperatur")



for i in range(len(vertices)):
    points.InsertPoint(i, vertices[i])

for i in range(len(areas)):
    polys.InsertNextCell( mkVtkIdList(areas[i]) )
    for j in range(len(areas[i])):
        scalars.InsertTuple1(areas[i][j]-1, Temperatures[index,i] )

output.SetPoints(points)
output.SetPolys(polys)

output.GetPointData().SetScalars(scalars)

output.GetInformation().Set(output.DATA_TIME_STEP(), req_time)

