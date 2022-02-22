set terminal png
set output "packet_througput.png"
set title "ThroughtPut calculation"
set xlabel "number of packets(/s)"
set ylabel "Throughput(Kbps)"
set yrange [0:150 < * < 150]
plot "packet_througput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5 , "packet_througput.txt" u 1:2 smooth bezier title 'throughput' lw 2 