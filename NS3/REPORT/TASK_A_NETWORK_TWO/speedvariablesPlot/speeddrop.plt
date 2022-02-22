set terminal png
set output "speed_drop.png"
set title "packet drop calculation"
set xlabel "speed(m/s)"
set ylabel "packet drop ratio(%)"
set yrange [0:8 < * < 8]
plot "speed_packetdrop.txt" using 1:2 with linespoints title "packet drop ratio" pt 7 ps 1.5 , "speed_packetdrop.txt" u 1:2 smooth bezier title 'packet drop ratio' lw 2, "speed_packetdrop.txt" u 1:2 smooth bezier title 'packet drop ratio' lw 2  