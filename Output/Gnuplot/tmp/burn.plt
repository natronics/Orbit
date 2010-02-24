#!/usr/bin/gnuplot -persist

reset

set xrange[0.0:4.5]
set out "Output/burnout_1.png"

load "./Output/Gnuplot/burn_base.plt"

set xrange[34.5:44.0]
set out "Output/burnout_2.png"

load "./Output/Gnuplot/burn_base.plt"

set xrange[150.0:242.4]
set out "Output/burnout_3.png"

load "./Output/Gnuplot/burn_base.plt"

#EOF