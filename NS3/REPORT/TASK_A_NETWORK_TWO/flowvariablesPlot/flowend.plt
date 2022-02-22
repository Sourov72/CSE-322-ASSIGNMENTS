set terminal png
set output "flow_end.png"
set title "End to End Delay calculation"
set xlabel "number of flows"
set ylabel "endtoendDelay(ms)"
set yrange [50:210 < * < 210]
plot "flow_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5, "flow_end.txt" u 1:2 smooth bezier title 'endToendDelay' lw 2