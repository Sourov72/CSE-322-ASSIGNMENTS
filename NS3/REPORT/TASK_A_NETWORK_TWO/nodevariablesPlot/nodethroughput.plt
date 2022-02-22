set terminal png
set output "nodes_througput.png"
set title "ThroughtPut calculation"
set xlabel "number of nodes"
set ylabel "Throughput(Kbps)"
set yrange [-10:50 < * < 50]
plot "nodes_throughput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5,"nodes_throughput.txt" u 1:2 smooth bezier title 'throughput' lw 2 