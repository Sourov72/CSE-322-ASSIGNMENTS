set terminal png
set output "end to end delay.png"
set title "End To End Delay calculation"
set xlabel "no of flows"
set ylabel "endToend(ms)"
plot "endToend.txt" using 1:2 with linespoints title "withoutalgo", "endToend.txt" using 1:3 with linespoints title "withalgo(14000bytesthresh)", "endToend.txt" using 1:4 with linespoints title "withalgo(30000bytesthresh)", "endToend.txt" using 1:5 with linespoints title "withalgo(default)"