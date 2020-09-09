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
    //#[swag.printbc]
    func print(using self, x: string)
    {
        G = true
    }
}

//#[swag.printbc]
#test
{
    G = false
    var y: TITI
    TITI.TOTO.print(y, "toto\n")
    @assert(G == true)
}