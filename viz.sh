#!/bin/bash

echo "Visualizing..."

gnuplot alt-t.plt
gnuplot alt-down.plt
gnuplot alt-burn.plt
#gnuplot acc-t.plt
gnuplot acc-burn.plt
#gnuplot vel-t.plt
gnuplot vel-burn.plt
gnuplot earth.plt

echo "Done."

