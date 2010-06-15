#!/usr/bin/gnuplot -persist

reset

set xrange[0:33.3]
set yrange[0:20.8]

l = 228
load "./Output/Gnuplot/overview_base.plt"
set autoscale y
load "./Output/Gnuplot/overview_vel.accel_base.plt"
#    EOF