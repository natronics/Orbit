#!/bin/bash

BILD_DIR=Build
 
if [ -f $BILD_DIR ];
then
   echo "Building..."
else
   echo "$BILD_DIR does not exist, creating..."
   mkdir $BILD_DIR
   echo "Building..."
fi

cd Source

gcc orbit.c physics.c vecmath.c coord.c rout.c rk4.c -lm -lconfig -o ../Build/orbit

echo "Done."

cd ..

