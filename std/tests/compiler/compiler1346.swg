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
    y.TOTO.print("toto\n")
    @assert(G == true)
}