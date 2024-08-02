#global testerror "Err0313"

struct TOTO
{
	print: func(self, bool)
}

func print1(s: *TOTO, x: bool)
{
}

//#[Swag.PrintBc]
#test
{
    var x: TOTO
    x.print = &print1
    TOTO.print(x, true)
}