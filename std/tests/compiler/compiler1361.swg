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
    //#[swag.printbc]
    func print(using self)
    {
        @assert(val == 666)
        G = 555
        //@print("TITI\n")
    }
}

//#[swag.printbc]
func printWhatever()
{
    itf.print()
}

var itf: IPrintable

//#[swag.printbc]
#test
{   
    G = 0
    var titi: TITI
    itf = titi
    printWhatever()
    @assert(G == 555)
}