#!/usr/bin/gnuplot -persist

unset key

set style line 1 lt 1 lw 2 lc rgb "#ff0000"
set style line 2 lt 1 lw 2 lc rgb "#444444"
set term png truecolor size 512,512
set out "Output/ascent-alt-down.png"

set ylabel "Altitude (MSL) [km]"
set xlabel "Downrange [km]"
plot "./Output/out-burn.dat" us ($18/1000.0):($17/1000.0) w lines linestyle 1, \
"./Output/out-coast.dat" us ($18/1000.0):($17/1000.0) w lines linestyle 2

#    EOF
