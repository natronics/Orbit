#!/usr/bin/gnuplot -persist

reset

set xrange[0:258.2]
set yrange[0:159.0]

l = 685
load "./Output/Gnuplot/overview_base.plt"
set autoscale y
load "./Output/Gnuplot/overview_vel.accel_base.plt"
#    EOF