struct TOTO
{
	print: func(self, bool)
    CPT: s32 = 666
}

var G: bool

//#[Swag.PrintBc]
func print(s: *TOTO, tt: bool)
{
    @assert(s.CPT == 666)
    G = true
}

//#[Swag.PrintBc]
#test
{
    G = false
    var XX: TOTO
    @assert(XX.CPT == 666)
    XX.print = &print
    XX.print(true)
    @assert(G == true)
}