#!/bin/bash

if [ $# -lt 3 ];
then
    print 'Expected 3 arguments'
    print 'simulation command, XML-Configuration and currentprofile'
    print 'e.g. ./frameworkBenchmark_ThEl testconfig.xml timeSeries.txt'

    exit
fi

simcmd=$1
config=$2
currentProfile=$3 

cycles=500
if [ $# -ge 4 ];
then
cycles=$4
fi

maxworker=500
if [ $# -ge 5 ];
then
maxworker=$5
fi


> benchmarkResults

for ((worker=1;worker<=$maxworker;worker++)) ; do

  mkdir -p $worker;
  cat $config | sed -- "s/DISC/$worker/g" > $worker/testblobb.xml
  echo "Run number: $worker"
  simout=`$simcmd $worker/testblobb.xml $3 $cycles`
  summary=`echo "$simout" | tail -n 4`

  meantime=`    echo "$summary" | grep Mittelwert | cut -d ":" -f 2`
  cycle=`       echo "$summary" | grep Cycles | cut -d ":" -f 2`
  obEle=`       echo "$summary" | grep Observed | cut -d ":" -f 2`
  echo "DAS IST DER SIMOUT: $simout" >> blobb

  echo "$meantime;$cycle;$obEle" >> benchmarkResults
done
