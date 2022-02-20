set terminal png
set output "area_delivery.png"
set title "packet delivery ratio calculation"
set xlabel "Area CoverAge(Tx*n)"
set ylabel "packet delivery ratio(%)"
set yrange [50:110 < * < 110]
plot "area_packetdelivery.txt" using 1:2 with linespoints title "packet delivery ratio" pt 7 ps 1.5 