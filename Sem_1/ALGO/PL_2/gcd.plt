# Gcd plot
#===============

# Uncomment the following two lines to generate a png file as output
#set terminal png 
#set output 'gcd.png'

# .. or these for eps files
set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'gcd.eps'

set title 'GCD'
set ylabel 'Time [ms]'
set xlabel 'Bit length of numbers'

# NAIVE ONLY:
plot 'gcdNaive.data' using 1:2 title 'Naive' with linespoints

# EUCLID ONLY:
#plot 'gcdEuclid.data' using 1:2 title 'Euclid' with linespoints

# EXTENDED ONLY:
#plot 'gcdExtended.data' using 1:2 title 'Extended Euclid' with linespoints

# EUCLID AND EXTENDED:
#plot 'gcdEuclid.data' using 1:2 title 'Euclid' with linespoints, 'gcdExtended.data' using 1:2 title 'Extended Euclid' with linespoints

# NAIVE, EUCLID AND EXTENDED:
#plot 'gcdNaive.data' using 1:2 title 'Naive' with linespoints, 'gcdEuclid.data' using 1:2 title 'Euclid' with linespoints, 'gcdExtended.data' using 1:2 title 'Extended Euclid' with linespoints

# Keep the result window open
pause -1
