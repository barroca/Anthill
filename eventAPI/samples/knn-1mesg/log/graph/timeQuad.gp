set term postscript eps 21 enhanced
set encoding iso_8859_1
set xlabel "Number of threads"
set ylabel "Time (seconds)"
set xtics 1
set boxwidth 0.4 absolute
set style fill solid 0.250000 border
set xrange [0:]
set yrange [0:]
set grid
set output "timeThreadKnnQuad.ps"
plot "timeQuad.txt"  using 1:2 notitle with boxes 
