set terminal png
set output "nodes_drop.png"
set title "packet drop calculation"
set xlabel "number of nodes"
set ylabel "packet drop ratio(%)"
set yrange [0:1 < * < 1]
plot "nodes_packetdrop.txt" using 1:2 with linespoints title "packet drop ratio" pt 7 ps 1.5 