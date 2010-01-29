#!/usr/bin/gnuplot -persist

unset key

f(x,y,z) = sqrt(x**2 + y**2 + z**2)

set style line 1 lt 1 lw 3 lc rgb "#dd2222"
set style line 2 lt 1 lw 3 lc rgb "#444444"
set style line 3 lt 0 lw 1 lc rgb "#aaaaaa"
set style line 4 lt 1 lw 2 lc rgb "#ccccff"
set term png font "/usr/share/fonts/truetype/Helvetica/Helvetica LT.ttf" 10 truecolor size 1100, 647
set out "Output/overview.png"

set ylabel "Altitude (MSL) [km]"
set xlabel "Downrange [km]"
set grid lc rgb "#cccccc"

plot "./Output/out-spentStages.dat" us ($18/1000.0):($17/1000.0) w lines linestyle 3, \
"./Output/out-coast.dat" us ($18/1000.0):($17/1000.0) w lines linestyle 2, \
"./Output/out-burn.dat" us ($18/1000.0):($17/1000.0) w lines linestyle 1

#    EOF
