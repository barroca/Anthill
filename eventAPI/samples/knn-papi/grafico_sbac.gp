set term postscript eps 
set title "Frequency Distribution of Session Size"
set output "h_sbac.eps"
set ylabel "Frequency" 
set xlabel "Session size (requests)" 
#set logscale y
#set logscale x
plot [:] "lau.txt" u 2:6 notitle with points
