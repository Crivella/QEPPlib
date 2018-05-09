#!/usr/bin/gnuplot

set term wxt enhanced
set encoding utf8
set termoption dashed

FILE="plotted.dat"

stats FILE u 1:5 nooutput

xmin = STATS_min_x
xmax = STATS_max_x

set xrange [xmin:xmax]
set yrange [-3:3]

set arrow 1 from xmin,0 to xmax,0 nohead dt(10,5) lc rgb "grey"

plot \
	for[i=2:200] FILE u 1:i w l notitle

#for[i=2:200] FILE u 1:i w l dt (15+25**(i%2),25) notitle




pause -1





