#global testerror "Err0645"

struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    @print(x.x)
}

//#[Swag.PrintBc]
#test
{
    toto({true})
}