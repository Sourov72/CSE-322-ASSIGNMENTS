set terminal png
set output "flow_drop.png"
set title "packet drop calculation"
set xlabel "number of flows"
set ylabel "packet drop ratio(%)"
set yrange [0:3 < * < 3]
plot "flow_packetdrop.txt" using 1:2 with linespoints title "packet drop ratio" pt 7 ps 1.5 