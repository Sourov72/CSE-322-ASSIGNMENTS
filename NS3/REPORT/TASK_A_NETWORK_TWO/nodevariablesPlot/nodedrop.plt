set terminal png
set output "nodes_drop.png"
set title "packet drop calculation"
set xlabel "number of nodes"
set ylabel "packet drop ratio(%)"
set yrange [0:100 < * < 100]
plot "nodes_packetdrop.txt" using 1:2 with linespoints title "packet drop ratio" pt 7 ps 1.5 ,"nodes_packetdrop.txt" u 1:2 smooth bezier title 'packet drop ratio' lw 2