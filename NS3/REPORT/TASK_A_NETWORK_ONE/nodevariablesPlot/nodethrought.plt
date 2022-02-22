set terminal png
set output "nodes_througput.png"
set title "ThroughtPut calculation"
set xlabel "number of nodes"
set ylabel "Throughput(Kbps)"
set yrange [6000:12000 < * < 12000]
plot "nodes_througput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5, "nodes_througput.txt" u 1:2 smooth bezier title 'throughput' lw 2 