#global testerror "Err0701"

//#[Swag.PrintBc]
#test
{
    var a, b: s32
    (a, b) = {1, 2, 3}
    @assert(a == 1)
    @assert(b == 2)
}