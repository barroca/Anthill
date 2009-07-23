set term postscript eps 21 enhanced color
set encoding iso_8859_1
set xlabel "Number of machines"
set ylabel "Value"
set xrange [:]
set yrange [:25]
set grid
set key left top
set output "speedupApriori.ps"
plot "max.txt"  using 1:3 title "Linear Speedup #cores" with linespoints 1, \
"max.txt"  using 1:2 title "Application Speedup" with linespoints 7
