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
    TITI.TOTO.print(y, "toto\n")
    @assert(G == true)
}