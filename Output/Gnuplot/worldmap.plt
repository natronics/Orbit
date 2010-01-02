#!/usr/bin/gnuplot -persist

reset
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
set style line 5 lt 1 lw 2 lc rgb "#ff0000" pt 4 ps 0.5
set style line 6 lt 1 lw 2 lc rgb "#eeee22" pt 4 ps 0.5

a = 6371.0

f(x,y) = a * cos(y) * sin(90 - x)
g(x,y) = a * sin(y) * sin(90 - x)
h(x,y) = a * cos(90 - x) 

set terminal png truecolor size 1100, 1100
set out "Output/worldmap.png"

set multiplot 

set origin 0,0
set size 0.5, 0.5
set view 60, 30, 1.5

splot a*cos(u)*cos(v),a*sin(u)*cos(v),a*sin(v) linestyle 4, \
"./Output/MapData/world.map" every 100 us (f($1,$2)):(g($1,$2)):(h($1,$2)) w lines linestyle 1, \
"./Output/MapData/rivers.map" every 150 us (f($1,$2)):(g($1,$2)):(h($1,$2)) w lines linestyle 2, \
"./Output/MapData/countries-us.map" every 40 us (f($1,$2)):(g($1,$2)):(h($1,$2)) w lines linestyle 3, \
"./Output/out-burn.dat" every 110 us ($3/1000.0):($4/1000.0):($5/1000.0) w lines linestyle 5 ,\
"./Output/out-coast.dat" every 110 us ($3/1000.0):($4/1000.0):($5/1000.0) w lines linestyle 6


set origin 0.5, 0
set size 0.5, 0.5

set view 120, 210, 1.5

splot a*cos(u)*cos(v),a*sin(u)*cos(v),a*sin(v) linestyle 4, \
"./Output/MapData/world.map" every 100 us (f($1,$2)):(g($1,$2)):(h($1,$2)) w lines linestyle 1, \
"./Output/MapData/rivers.map" every 150 us (f($1,$2)):(g($1,$2)):(h($1,$2)) w lines linestyle 2, \
"./Output/MapData/countries-us.map" every 40 us (f($1,$2)):(g($1,$2)):(h($1,$2)) w lines linestyle 3, \
"./Output/out-burn.dat" every 110 us ($3/1000.0):($4/1000.0):($5/1000.0) w lines linestyle 5 ,\
"./Output/out-coast.dat" every 110 us ($3/1000.0):($4/1000.0):($5/1000.0) w lines linestyle 6

set origin 0, 0.5
set size 1, 0.5

set xtics 15
set ytics 15
set ylabel "Latitude [deg]"
set xlabel "Longitude [deg]"
set grid lc rgb "#cccccc"

plot "./Output/MapData/world.map" every 100 us 2:1 w lines linestyle 1, \
"./Output/MapData/rivers.map" every 150 us 2:1 w lines linestyle 2, \
"./Output/MapData/countries-us.map" every 50 us 2:1 w lines linestyle 3, \
"./Output/out-burn.dat" us 16:15 w points linestyle 5 ,\
"./Output/out-coast.dat" us 16:15 w points linestyle 6

unset multiplot
#    EOF
