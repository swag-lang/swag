interface TOTO
{
	print: func(self, string)
}

struct TITI
{
}

var G: bool

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func print(using self, x: string)
    {
        G = true
    }
}

//#[Swag.PrintBc]
#test
{
    G = false
    var y: TITI
    y.TOTO.print("toto\n")
    @assert(G == true)
}