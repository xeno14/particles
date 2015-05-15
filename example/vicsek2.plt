do for [i=0:3000] {
  plot[0:16][0:16] "vicsek2.dat" index i usi 1:2:3:4 w vec title sprintf("%d-th step", i)
  pause -1
}
