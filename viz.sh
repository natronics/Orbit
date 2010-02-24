#!/bin/bash

echo "Visualizing..."

gnuplot Output/Gnuplot/ascent.plt
gnuplot Output/Gnuplot/ascent-alt.plt
gnuplot Output/Gnuplot/ascent-toApogee.plt
gnuplot Output/Gnuplot/worldmap.plt
gnuplot Output/Gnuplot/tmp/ascent-alt-down.plt
gnuplot Output/Gnuplot/tmp/launchmap.plt
gnuplot Output/Gnuplot/tmp/overview.plt
gnuplot Output/Gnuplot/tmp/burn.plt
gnuplot Output/Gnuplot/tmp/launch-3d.plt

echo "Done."

