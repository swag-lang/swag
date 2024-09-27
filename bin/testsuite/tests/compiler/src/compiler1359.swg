interface IPrintable
{
	func print(self);
}

struct TITI
{
    val: s64 = 666
}

var G = 0

impl IPrintable for TITI
{
    //#[Swag.PrintBc]
    func impl print(using self)
    {
        @assert(val == 666)
        G = 555
        //@print("TITI\n")
    }
}

//#[Swag.PrintBc]
func printWhatever(_itf: nn *IPrintable)
{
    _itf.print()
}

//#[Swag.PrintBc]
#test
{
    G = 0
    var titi: TITI
    var itf: IPrintable
    itf = titi
    printWhatever(&itf)
    @assert(G == 555)
}