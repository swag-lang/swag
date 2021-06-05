
#global testerror

struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    @print(x.x)
}

//#[Swag.printbc]
#test
{
    toto(@{true})
}