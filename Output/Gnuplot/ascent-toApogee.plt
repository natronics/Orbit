#!/usr/bin/gnuplot -persist

reset
unset key

set format x ""
set format y "%10.0f"
set tmargin 0
set bmargin 0

f(x,y,z) = sqrt(x**2 + y**2 + z**2)

set style line 1 lt 1 lw 2 lc rgb "#ff0000"
set style line 2 lt 1 lw 2 lc rgb "#eeee22"

set term png truecolor size 800,1024
set out "Output/ascent-toApogee.png"

set multiplot
set origin 0,0.6
set size 1, 0.4
set tmargin 4
set ylabel "Altitude (MSL) [km]"
plot "./Output/out-burn.dat" us 1:($17/1000.0) w lines linestyle 1, \
"./Output/out-coast.dat" us 1:($17/1000.0) w lines linestyle 2

set origin 0, 0.3
set size 1, 0.3
set tmargin 0
set ylabel "Velocity (norm) [m/s]"
plot "./Output/out-burn.dat" us 1:(f($6,$7,$8)) w lines linestyle 1, \
"./Output/out-coast.dat" us 1:(f($6,$7,$8)) w lines linestyle 2

set origin 0,0
set size 1, 0.3
set format x "%.0f"
set bmargin 4
set ylabel "Accleration (norm) [g]"
plot "./Output/out-burn.dat" us 1:(f($9,$10,$11) / 9.8) w lines linestyle 1, \
"./Output/out-coast.dat" us 1:(f($9,$10,$11) / 9.8) w lines linestyle 2

unset multiplot
#    EOF
