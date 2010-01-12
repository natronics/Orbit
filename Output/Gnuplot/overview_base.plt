#!/usr/bin/gnuplot -persist

unset key

set style line 1 lt 1 lw 2 lc rgb "#dd2222"
set style line 2 lt 1 lw 2 lc rgb "#444444"
set style line 3 lt 0 lw 2 lc rgb "#aaaaaa"
set term png truecolor size 1100, 647
set out "Output/overview.png"

set ylabel "Altitude (MSL) [km]"
set xlabel "Downrange [km]"
plot "./Output/out-spentStages.dat" us ($18/1000.0):($17/1000.0) every 10 w lines linestyle 3, \
"./Output/out-coast.dat" us ($18/1000.0):($17/1000.0) every 10 w lines linestyle 2, \
"./Output/out-burn.dat" us ($18/1000.0):($17/1000.0) every 10 w lines linestyle 1



#    EOF
