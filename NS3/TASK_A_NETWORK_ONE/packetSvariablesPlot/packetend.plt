set terminal png
set output "packet_end.png"
set title "End to End Delay calculation"
set xlabel "number of packets(/s)"
set ylabel "endtoendDelay(ms)"
set yrange [0:150 < * < 150]
plot "packet_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5 