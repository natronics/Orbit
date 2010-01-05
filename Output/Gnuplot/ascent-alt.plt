#!/usr/bin/gnuplot -persist

reset
unset key

set style line 1 lt 1 lw 2 lc rgb "#ff0000"
set term png truecolor size 512,512
set out "Output/ascent-alt.png"

set ylabel "Altitude (MSL) [km]"
set xlabel "MET [seconds]"
plot "./Output/out-burn.dat" us 1:($17/1000.0) w lines linestyle 1

#    EOF
