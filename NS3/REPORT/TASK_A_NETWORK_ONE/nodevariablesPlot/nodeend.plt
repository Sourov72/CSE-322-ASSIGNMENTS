set terminal png
set output "nodes_end.png"
set title "End to End Delay calculation"
set xlabel "number of nodes"
set ylabel "endtoendDelay(ms)"
set yrange [100:230 < * < 230]
plot "nodes_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5, "nodes_end.txt" u 1:2 smooth bezier title 'endToendDelay' lw 2  