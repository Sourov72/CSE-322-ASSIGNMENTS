set terminal png
set output "delivery.png"
set title "Packet Delivery Ratio calculation"
set xlabel "no of flows"
set ylabel "Packet delivery ratio(%)"
plot "packetdelivery.txt" using 1:2 with linespoints title "withoutalgo", "packetdelivery.txt" using 1:3 with linespoints title "withalgo(14000bytesthresh)", "packetdelivery.txt" using 1:4 with linespoints title "withalgo(30000bytesthresh)", "packetdelivery.txt" using 1:5 with linespoints title "withalgo(default)"