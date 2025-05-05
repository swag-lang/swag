#global testerror "Err0040";
func main ()-> rune { return '1'; }
#assert(main() / '1');