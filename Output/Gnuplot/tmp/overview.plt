#!/usr/bin/gnuplot -persist

reset

set xrange[0:7.8]
set yrange[0:5.2]

l = 207
load "./Output/Gnuplot/overview_base.plt"
set autoscale y
load "./Output/Gnuplot/overview_vel.accel_base.plt"
#    EOF