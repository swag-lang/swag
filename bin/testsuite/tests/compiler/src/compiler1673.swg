#global testerror "Err0093"

struct TT
{
    value:  s64 = 456
}

//#[Swag.PrintBc]
func C(x: ...)
{
    //@print(#typeof(x[0]).name)
    var ptr1 = cast(const TT) x[0]
    ptr1.value = 456
}

#run
{
    var x: TT
    C(x)
}