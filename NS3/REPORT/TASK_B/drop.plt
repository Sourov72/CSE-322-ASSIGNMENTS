set terminal png
set output "drop.png"
set title "Packet Drop Ratio calculation"
set xlabel "no of flows"
set ylabel "Packet drop ratio(%)"
plot "packetloss.txt" using 1:2 with linespoints title "withoutalgo", "packetloss.txt" using 1:3 with linespoints title "withalgo(14000bytesthresh)", "packetloss.txt" using 1:4 with linespoints title "withalgo(30000bytesthresh)", "packetloss.txt" using 1:5 with linespoints title "withalgo(default)"