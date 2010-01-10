#!/usr/bin/gnuplot -persist

unset key
set parametric
set urange [ 0.00000 : 360.000 ] noreverse nowriteback
set vrange [ -90.0000 : 90.0000 ] noreverse nowriteback
set samples 100, 100
set isosamples 48, 24
set angles degrees
set hidden3d

unset border
unset tics

set style line 1 lt 1 lw 0.3 lc rgb "#77aa88"
set style line 2 lt 1 lw 0.3 lc rgb "#77bbcc"
set style line 3 lt 3 lw 0.3 lc rgb "#555555"
set style line 4 lt 1 lw 0.3 lc rgb "#bbcccc"
set style line 5 lt 1 lw 3 lc rgb "#ff0000" pt 4 ps 0.5
set style line 6 lt 1 lw 3 lc rgb "#eeee22" pt 4 ps 0.5

a = 6371.0

f(x,y) = a * cos(y) * sin(90 - x)
g(x,y) = a * sin(y) * sin(90 - x)
h(x,y) = a * cos(90 - x) 


set xtics 1
set ytics 1
set ylabel "Latitude [deg]"
set xlabel "Longitude [deg]"
set grid lc rgb "#cccccc"

set terminal png truecolor size 1024, 1024
set out "Output/launchmap.png"

plot "./Output/MapData/world.map" every 5 us 2:1 w lines linestyle 1, \
"./Output/MapData/rivers.map" every 5 us 2:1 w lines linestyle 2, \
"./Output/MapData/countries-us.map" us 2:1 w lines linestyle 3, \
"./Output/out-burn.dat" us 16:15 w lines linestyle 5 ,\
"./Output/out-coast.dat" us 16:15 w lines linestyle 6


#    EOF
