#!/usr/bin/gnuplot

#set term wxt enhanced
set term pdfcairo enhanced size 20cm,10cm

FILE="qepp_cd.dat"
OUT="charge_density.pdf"

clines=8
au=5.2917721067e-11

if( exist("f")) {
	FILE=sprintf("%s.dat",f)
	OUT= sprintf("%s.pdf",f)
}

set output OUT



stats FILE u 1:5 nooutput
zmax = STATS_max_y
stats FILE u (column(1)/au):(column(2)/au) nooutput
xmax = STATS_max_x
ymax = STATS_max_y

do for[i=0:zmax] {
	stats FILE u (column(1)/au):(column(5)==i?column(3)/au:1/0) nooutput
	curr_z = STATS_min_y

	set pm3d
	set dgrid3d 40,40
	set isosample 250, 250
	set table '1.table'
	splot FILE u (column(1)/au):(column(2)/au):(column(5)==i?column(4)*au**3:1/0)
	unset table
 
	set contour base
	set cntrparam level auto clines
	unset surface
	set table '2.table'
	splot FILE u (column(1)/au):(column(2)/au):(column(5)==i?column(4)*au**3:1/0)
	unset table
	
	reset
	set key
	set xrange [0:xmax]
	set yrange [0:ymax]
	set xlabel "x(a.u)"
	set ylabel "y(a.u)"
	set zlabel "n"

	set dgrid3d 40,40
	set pm3d
	unset colorbox

	set multiplot layout 1,2 title sprintf("z = %g", curr_z)
	set view 45,10
	set ztics
	splot	FILE u (column(1)/au):(column(2)/au):(column(5)==i?column(4)*au**3:1/0) w lp pt 7 ps 0.1 notitle
	#set view 0,0
	if (GPVAL_TERM ne 'wxt') { unset ztics }
	#replot
	unset key; unset xlabel; unset ylabel
	#set palette rgb 34,35,36 #maxcolors clines
	p '1.table' with image, '2.table' w l lt -1 lw 1.5 title sprintf("%d",i)
	unset multiplot

	if (GPVAL_TERM eq 'wxt') { pause -1 }
}

reset
set xrange [0:xmax]
set yrange [0:ymax]
set xlabel "x(a.u)"
set ylabel "y(a.u)"
set zlabel "n"

set dgrid3d 40,40
set pm3d
unset colorbox

set multiplot layout 1,2
set view 45,10
set ztics
splot 	FILE u (column(1)/au):(column(2)/au):(column(6)) w lp pt 7 ps 0.1 title "xy projected"
set view 0,0
unset ztics
replot
unset multiplot

#splot	'prova1.dat' u (column(1)*1.e+10):(column(2)*1.e+10):(column(3)==0?column(4):1/0) w lp pt 7 ps 0.1 title "2", \
	#'prova2.dat' u (column(1)*1.e+10):(column(2)*1.e+10):(column(4)) w lp pt 7 ps 0.1 notitle, \
	#'prova.dat' u (column(1)*1.e+10):(0):(column(4)*3E+32) w lp pt 6 ps 0.1 notitle



