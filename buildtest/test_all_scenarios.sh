#!/bin/bash -e

array=( $@ )
len=${#array[@]}

allArgumentsButTheLast=${array[@]:2:$len}
echo $allArgumentsButTheLast

if [ $len -lt 2 ]
then
    echo "==========================================================="
    echo "test_all_scenarios.sh testSuite XML-Dir AdditionalParameter"
    echo "==========================================================="
    echo "Not enough arguments"
    echo "First argument test suit"
    echo "Second argument XML-directory"
    exit
fi

for i in `ls $2/*.xml`
do 
    $1 $i $allArgumentsButTheLast
done
