set title "Altitude During Burn"
set xlabel "Time [s]"
set ylabel "Alt [m]"
set xrange[0:152.300002]
set term postscript color
set out "alt-burn.eps"
plot "./out.dat" us 1:16 notitle w lines lw 2
