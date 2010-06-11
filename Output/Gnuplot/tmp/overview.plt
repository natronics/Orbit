#!/usr/bin/gnuplot -persist

reset

set xrange[0:1.0]
set yrange[0:1.0]

l = 207
load "./Output/Gnuplot/overview_base.plt"
set autoscale y
load "./Output/Gnuplot/overview_vel.accel_base.plt"
#    EOF