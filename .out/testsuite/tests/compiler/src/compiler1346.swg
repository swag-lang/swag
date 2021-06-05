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
    //#[Swag.printbc]
    func print(using self, x: string)
    {
        G = true
    }
}

//#[Swag.printbc]
#test
{
    G = false
    var y: TITI
    y.TOTO.print("toto\n")
    @assert(G == true)
}