#!/bin/bash

BENCHMARKFOLDER=$1
BENCHMARK=$2
CURRENTPROFILE=$3 
VISIUALIZE_COMMAND=""

function PrintErrorMessage
{
    >&2 echo -en "\033[1;31mERROR: $1\033[0m\n"
}

function PrintWarningMessage
{
    >&2 echo -en "\033[1;31mWarning: $1\033[0m\n"
}

function PrintMessage
{
    echo -en "\033[1;32m$1\033[0m\n"
}

function GetVisualiserCommand
{
    command -v matlab >/dev/null 2>&1
    if [ $? -ne 1 ]; then
        echo "matlab -nodisplay -nosplash -r"
    else
        command -v octave >/dev/null 2>&1
        if [ $? -ne 1 ]; then
            echo "octave --eval"
        else
            PrintErrorMessage "Couldn't find matlab nor octave"
            exit 1
        fi
    fi
}

function main
{
    VISIUALIZE_COMMAND=$(GetVisualiserCommand)
    #echo ${VISIUALIZE_COMMAND}

    TESTSUITS=`ls ${BENCHMARKFOLDER}`
    for i in $TESTSUITS; do 
        #echo "Running ${BENCHMARKFOLDER}/${i}"
        ./benchmarkmakerSingleTest.sh ${BENCHMARK} ${BENCHMARKFOLDER}/${i} ${CURRENTPROFILE} 10 10 > /dev/null
        filename=$(basename "$i")
        filename="${filename%.*}"

        ${VISIUALIZE_COMMAND} "x=dlmread('benchmarkResults', ';'); plot(x(:,3), x(:,1)); print -djpg image.jpg1"
        mv benchmarkResults ${filename}.csv
    done
}

main $@
