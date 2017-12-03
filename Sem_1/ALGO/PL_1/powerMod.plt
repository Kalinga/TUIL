# Power mod plot
#===============

# Uncomment the following two lines to generate a png file as output
#set terminal png 
#set output 'powerMod.png'

# .. or these for eps files
set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'powerMod.eps'

set title 'Fast modular exponentiation'
set ylabel 'Time [ms]'
set xlabel 'Bit length of numbers'

# Plot data from the created file "powerMod.data" and also the polynomials 0.0005*n^3 and 0.000250*n^3
plot 'powerMod.data' using 1:2 title 'Fast modular exponentiation' with linespoints, 0.0005*x**3 title '0.0005*n^3', 0.000250*x**3 title '0.000250*n^3'

# Keep the result window open
pause -1
