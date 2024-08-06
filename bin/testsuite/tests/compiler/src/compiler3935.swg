#global testerror "Err0630"

//#[Swag.PrintBc]
#test
{
    var a = 2'f32
    var b: const &s32 = &a
    @assert(b == 2)
}
