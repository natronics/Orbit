unset key
set hidden

set style line 1 lt 1 lw 0.6 lc rgb "#77aa88"
set style line 2 lt 1 lw 0.6 lc rgb "#77bbcc"
set style line 3 lt 0 lw 0.6 lc rgb "#aaaaaa"
set style line 4 lt 1 lw 0.3 lc rgb "#bbcccc"
set style line 5 lt 1 lw 3 lc rgb "#dd2222"
set style line 6 lt 1 lw 3 lc rgb "#000000"
set style line 7 lt 0 lw 1 pt 2 ps 2 lc rgb "#aa22ff"
set style line 8 lt 0 lw 1 lc rgb "#4444dd"
set style line 9 lt 1 lw 1 lc rgb "#999999"

set view 70, 300, 6.1, 1.4

lonTrace(x,z) = x + (z / 100000)
latTrace(y,z) = y + (z / 100000)

lon1Trace(x,z) = x - (z / 100000)
lat1Trace(y,z) = y - (z / 100000)

set term png font "/usr/share/fonts/truetype/Helvetica/Helvetica LT.ttf" 10 truecolor size 1100, 1100
set out "Output/launch-3d.png"


set zrange[0:6000]

splot "./Output/MapData/world.map" us 2:1:(0) every 10 w lines linestyle 1, \
"./Output/MapData/rivers.map" us 2:1:(0) every 10 w lines linestyle 2, \
"./Output/MapData/countries-us.map" us 2:1:(0) every 10 w lines linestyle 9, \
(0) linestyle 3, \
"./Output/out-coast.dat" us 16:15:($17/1000) every ::::1000 w lines linestyle 6, \
"./Output/out-spentStages.dat" us 16:15:($17/1000) w lines linestyle 3, \
"./Output/out-burn.dat" us 16:15:($17/1000) w lines linestyle 5, \
"./Output/out-coast.dat" us 16:15:(0) every ::::1000 w lines linestyle 7, \
"./Output/out-burn.dat" us 16:15:(0) w lines linestyle 7, \
"./Output/out-burn.dat" us (lonTrace($16,$17)):(latTrace($15, $17)):(0) w lines linestyle 8, \
"./Output/out-coast.dat" us (lonTrace($16,$17)):(latTrace($15, $17)):(0) every ::::1000 w lines linestyle 8, \
"./Output/out-burn.dat" us (lon1Trace($16,$17)):(lat1Trace($15, $17)):(0) w lines linestyle 8, \
"./Output/out-coast.dat" us (lon1Trace($16,$17)):(lat1Trace($15, $17)):(0) every ::::1000 w lines linestyle 8

# EOF
