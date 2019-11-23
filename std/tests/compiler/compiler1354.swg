struct TOTO
{
    //CPT: s32 = 666
    print1: func(self, string)
	print: func(self, string)
}

func print1(s: *TOTO, x: string)
{
    //@assert(s.CPT == 666)
    @assert(x == "true")
}

//#[swag.printbc]
#test
{
    var x: TOTO
    x.print = &print1   
    x.print("true")
}