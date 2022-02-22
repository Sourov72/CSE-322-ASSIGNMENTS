set terminal png
set output "Throughput.png"
set title "ThroughtPut calculation"
set xlabel "no of flows"
set ylabel "Throughput(kbps)"
plot "throughput.txt" using 1:2 with linespoints title "withoutalgo", "throughput.txt" using 1:3 with linespoints title "withalgo(14000bytesthresh)", "throughput.txt" using 1:4 with linespoints title "withalgo(30000bytesthresh)", "throughput.txt" using 1:5 with linespoints title "withalgo(default)"