interface IPrintable
{
	func print(self);
}

struct TITI
{
    val: s64 = 666
}

struct CONTENT
{
    itf: *IPrintable
}

var G = 0

impl IPrintable for TITI
{
    //#[Swag.PrintBc]
    func impl print(using self)
    {
        @assert(val == 666)
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0
    var titi: TITI
    var content: CONTENT

    var itf0: IPrintable
    content.itf = &itf0
    itf0 = titi

    content.itf.print()
    @assert(G == 1)
}