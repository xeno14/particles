do for [i=0:100] {
  splot[0:4][-2:2][-1:3] "uvlm.dat" index i usi 1:2:3 title sprintf("%d-th step", i)
  pause -1
}
