#!/bin/bash

echo "Visualizing..."

gnuplot alt-t.plt
gnuplot acc-t.plt
gnuplot alt-down.plt
gnuplot vel-t.plt
gnuplot earth.plt

echo "Done."

