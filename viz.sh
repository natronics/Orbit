#!/bin/bash

echo "Visualizing..."

gnuplot Output/Gnuplot/ascent.plt
gnuplot Output/Gnuplot/ascent-toApogee.plt
gnuplot Output/Gnuplot/worldmap.plt
gnuplot Output/Gnuplot/launchmap.plt

echo "Done."

