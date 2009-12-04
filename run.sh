#!/bin/bash

./clean.sh

./build.sh

echo "Running..." 

./Build/orbit -c sample.cfg

echo "Done."

./viz.sh

