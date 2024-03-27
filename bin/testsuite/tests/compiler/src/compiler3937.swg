#global testerror "Err0104"

//#[Swag.PrintBc]
#test
{
    var a = 2
    var b: const &s32 = &a
    b = 3
    @assert(a == 3)
    @assert(b == 3)
}
