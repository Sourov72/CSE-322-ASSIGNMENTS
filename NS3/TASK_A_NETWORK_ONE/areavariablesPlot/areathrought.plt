set terminal png
set output "area_througput.png"
set title "ThroughtPut calculation"
set xlabel "Area CoverAge(Tx*n)"
set ylabel "Throughput(Kbps)"
set yrange [7000:10000 < * < 15000]
plot "area_througput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5 