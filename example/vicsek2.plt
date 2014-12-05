do for [i=0:999] {
  #title "Vicsek model for 2D"
  plot[0:16][0:16] "out.dat" index i using 1:2:($3*3):($4*3) with vector title sprintf("%d-th step", i)
  pause -1
}
