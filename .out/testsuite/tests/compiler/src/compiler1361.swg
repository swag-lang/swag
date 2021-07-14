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
    //#[Swag.PrintBc]
    func print(using self)
    {
        @assert(val == 666)
        G = 555
        //@print("TITI\n")
    }
}

//#[Swag.PrintBc]
func printWhatever()
{
    itf.print()
}

var itf: IPrintable

//#[Swag.PrintBc]
#test
{
    G = 0
    var titi: TITI
    itf = titi
    printWhatever()
    @assert(G == 555)
}