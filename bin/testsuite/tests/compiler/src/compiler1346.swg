interface TOTO
{
	func print(self, x: string);
}

struct TITI
{
}

var G: bool

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func impl print(using self, x: string)
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