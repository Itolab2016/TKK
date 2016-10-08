set grid;
plot "test.txt" u 4:1 w l;
set output "accelx,accelx.eps" ;
replot;
set terminal;
set output;

