set terminal png
set output "speed_end.png"
set title "End to End Delay calculation"
set xlabel "speed(m/s)"
set ylabel "endtoendDelay(ms)"
set yrange [0:100 < * < 100]
plot "speed_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5 , "speed_end.txt" u 1:2 smooth bezier title 'endToendDelay' lw 2 