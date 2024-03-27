//
//#global testerror

struct TT
{
    value: s64 = 456
}

//#[Swag.PrintBc]
func C(x: ...)
{
}

#run
{
    var x: TT
    C(x)
}
