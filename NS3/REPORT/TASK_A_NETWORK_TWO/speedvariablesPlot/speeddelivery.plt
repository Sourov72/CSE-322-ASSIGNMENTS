set terminal png
set output "speed_delivery.png"
set title "packet delivery ratio calculation"
set xlabel "speed(m/s)"
set ylabel "packet delivery ratio(%)"
set yrange [50:110 < * < 110]
plot "speed_packetdelivery.txt" using 1:2 with linespoints title "packet delivery ratio" pt 7 ps 1.5 , "speed_packetdelivery.txt" u 1:2 smooth bezier title 'packet delivery ratio' lw 2 