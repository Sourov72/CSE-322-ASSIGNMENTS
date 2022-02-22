set terminal png
set output "packet_delivery.png"
set title "packet delivery ratio calculation"
set xlabel "number of packets(/s)"
set ylabel "packet delivery ratio(%)"
set yrange [50:110 < * < 110]
plot "packet_packetdelivery.txt" using 1:2 with linespoints title "packet delivery ratio" pt 7 ps 1.5, "packet_packetdelivery.txt" u 1:2 smooth bezier title 'packet delivery ratio' lw 2  