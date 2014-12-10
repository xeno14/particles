#!/usr/bin/env gnuplot

do for [i=0:999] {
  L=16
  set xrange[-L:L*2]
  set yrange[-L:L*2]
  set zrange[-L:L*2]
  splot "chate3.dat" index i using 1:2:3:($4*6):($5*6):($6*6) with vector title sprintf("%d-th step", i)
  pause -1
}
