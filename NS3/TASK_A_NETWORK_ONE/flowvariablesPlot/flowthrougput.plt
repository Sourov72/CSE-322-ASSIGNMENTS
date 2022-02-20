set terminal png
set output "flow_througput.png"
set title "ThroughtPut calculation"
set xlabel "number of flows"
set ylabel "Throughput(Kbps)"
set yrange [6000:15000 < * < 15000]
plot "flow_througput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5 