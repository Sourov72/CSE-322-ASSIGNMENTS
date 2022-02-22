set terminal png
set output "flow_througput.png"
set title "ThroughtPut calculation"
set xlabel "number of flows"
set ylabel "Throughput(Kbps)"
set yrange [0:250 < * < 250]
plot "flow_throughput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5,"flow_throughput.txt" u 1:2 smooth bezier title 'throughput' lw 2 