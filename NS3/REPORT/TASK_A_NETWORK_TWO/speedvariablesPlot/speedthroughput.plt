set terminal png
set output "speed_througput.png"
set title "ThroughtPut calculation"
set xlabel "speed(m/s)"
set ylabel "Throughput(Kbps)"
set yrange [10:50 < * < 50]
plot "speed_througput.txt" using 1:2 with linespoints title "throughput" pt 7 ps 1.5 , "speed_througput.txt" u 1:2 smooth bezier title 'throughput' lw 2 