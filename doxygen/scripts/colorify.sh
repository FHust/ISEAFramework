#!/bin/bash -e
if [ "$#" -ne 1 ]; then
    echo -e "\033[1;31mIllegal number of parameters\e[0m"
    exit 1
fi

DIRNAME=$(dirname "$1")
FILENAME=$(basename "$1")
EXTENSION="${FILENAME##*.}"
FILENAME="${FILENAME%.*}"
COLORED_FILENAME="$DIRNAME/${FILENAME}_color.$EXTENSION"

## DEBUG OUTPUT
#echo $DIRNAME
#echo $FILENAME 
#echo $EXTENSION
#echo $COLORED_FILENAME

COLORED_OUTPUT=`/usr/bin/source-highlight -s xml --input $1 -o STDOUT`
COLORED_OUTPUT='<div class="fragment">\n'${COLORED_OUTPUT}'\n</div>'
echo -e "$COLORED_OUTPUT" > $COLORED_FILENAME

