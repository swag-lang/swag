interface IPrintable
{
	print: func(self)
}

struct TITI
{
    val: s64 = 666
}

var G = 0

impl IPrintable for TITI
{
    //#[Swag.printbc]
    func print(using self)
    {
        @assert(val == 666)
        G = 555
        //@print("TITI\n")
    }
}

//#[Swag.printbc]
func printWhatever(_itf: *IPrintable)
{
    _itf.print()
}

var itf: IPrintable

//#[Swag.printbc]
#test
{
    G = 0
    var titi: TITI
    itf = titi
    printWhatever(&itf)
    @assert(G == 555)
}