set terminal png
set output "area_end.png"
set title "End to End Delay calculation"
set xlabel "Area CoverAge(Tx*n)"
set ylabel "endtoendDelay(ms)"
set yrange [0:100 < * < 100]
plot "area_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5 