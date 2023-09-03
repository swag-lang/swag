#global testerror "Err0564"

//#[Swag.PrintBc]
#test
{
    var a = 2
    var b: const ref s32 = &a
    b = 3
    @assert(a == 3)
    @assert(b == 3)
}
