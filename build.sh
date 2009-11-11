#!/bin/bash

cd Source

echo "Building..."

gcc orbit.c physics.c vecmath.c coord.c rout.c rk4.c -lm -lconfig -o ../Build/orbit

echo "Done."

cd ..

