set terminal png
set output "area_througput.png"
set title "ThroughtPut calculation"
set xlabel "Area CoverAge(Tx*n)"
set ylabel "Throughput(Kbps)"
set yrange [2000:10000 < * < 15000]
plot "area_througput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5, "area_througput.txt" u 1:2 smooth bezier title 'throughput' lw 2  