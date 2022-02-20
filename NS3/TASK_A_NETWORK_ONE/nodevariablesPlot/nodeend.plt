set terminal png
set output "nodes_end.png"
set title "End to End Delay calculation"
set xlabel "number of nodes"
set ylabel "endtoendDelay(ms)"
set yrange [0:100 < * < 100]
plot "nodes_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5 