set terminal png
set output "area_drop.png"
set title "packet drop calculation"
set xlabel "Area CoverAge(Tx*n)"
set ylabel "packet drop ratio(%)"
set yrange [0:1.1 < * < 1.1]
plot "area_packetdrop.txt" using 1:2 with linespoints title "packet drop ratio" pt 7 ps 1.5, "area_packetdrop.txt" u 1:2 smooth bezier title 'packet drop ratio' lw 2 