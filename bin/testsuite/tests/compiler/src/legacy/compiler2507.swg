//#global testerror

//#[Swag.PrintBc, Swag.Optimize("bc", false)]
#test
{
    var x: [..] s32
    var v  = [10, 20]
    x = v
    @assert(@countof(x) == 2)
    //@assert(x[0] == 10)
}
