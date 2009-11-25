set title "Acceleration During Burn"
set xlabel "Time [s]"
set ylabel "Accel [m/s^2"
set xrange[0:152.300002]
set term postscript color
set out "acc-burn.eps"
plot "./out.dat" us 1:(sqrt($8**2 + $9**2 + $10**2)) notitle w lines lw 2
