#!/usr/bin/env gnuplot

do for [i=0:999] {
  L=16
  splot[0:L][0:L][0:L] "chate3.dat" index i using 1:2:3:($4*6):($5*6):($6*6) with vector title sprintf("%d-th step", i)
  pause -1
}
