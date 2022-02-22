set terminal png
set output "packet_drop.png"
set title "packet drop calculation"
set xlabel "number of packets(/s)"
set ylabel "packet drop ratio(%)"
set yrange [0:5 < * < 5]
plot "packet_packetdrop.txt" using 1:2 with linespoints title "packet drop ratio" pt 7 ps 1.5, "packet_packetdrop.txt" u 1:2 smooth bezier title 'packet drop ratio' lw 2  