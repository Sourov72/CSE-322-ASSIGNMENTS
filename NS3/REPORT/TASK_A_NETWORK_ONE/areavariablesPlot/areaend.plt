set terminal png
set output "area_end.png"
set title "End to End Delay calculation"
set xlabel "Area CoverAge(Tx*n)"
set ylabel "endtoendDelay(ms)"
set yrange [0:210 < * < 210]
plot "area_end.txt" using 1:2 with linespoints title "endToendDelay" pt 7 ps 1.5, "area_end.txt" u 1:2 smooth bezier title 'endToendDelay' lw 2 

