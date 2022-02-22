set terminal png
set output "nodes_delivery.png"
set title "packet delivery ratio calculation"
set xlabel "number of nodes"
set ylabel "packet delivery ratio(%)"
set yrange [70:110 < * < 110]
plot "nodes_packetdelivery.txt" using 1:2 with linespoints title "packet delivery ratio" pt 7 ps 1.5, "nodes_packetdelivery.txt" u 1:2 smooth bezier title 'packet delivery ratio' lw 2  