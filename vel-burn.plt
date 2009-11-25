set title "Velocity During Burn"
set xlabel "Time [s]"
set ylabel "Vel [m/s]"
set xrange[0:152.300002]
set term postscript color
set out "vel-burn.eps"
plot "./out.dat" us 1:(sqrt($5**2 + $6**2 + $7**2)) notitle w lines lw 2
