interface TOTO
{
	func print(self, x: bool);
}

struct TITI
{
    val: s64 = 666
}

struct TATA
{
    val: string = "TATA"
    val1: string = "TATA"
    val2: string = "TATA"
}

var G = 0

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func impl print(using self, tt: bool)
    {
        @assert(val == 666)
        G += 1
    }
}

impl TOTO for TATA
{
    //#[Swag.PrintBc]
    func impl print(using self, tt: bool)
    {
        @assert(val == "TATA")
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0

    var XX: TOTO
    var YY: TITI
    XX = YY
    XX.print(true)

    var ZZ: TATA
    XX = ZZ
    XX.print(true)

    @assert(G == 2)
}