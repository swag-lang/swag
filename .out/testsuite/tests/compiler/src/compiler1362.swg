interface IPrintable
{
	print: func(self)
}

struct TITI
{
    val: s64 = 666
}

struct CONTENT
{
    itf: IPrintable
}

var G = 0

impl IPrintable for TITI
{
    //#[Swag.PrintBc]
    func print(using self)
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
    content.itf = titi
    content.itf.print()
    @assert(G == 1)
}