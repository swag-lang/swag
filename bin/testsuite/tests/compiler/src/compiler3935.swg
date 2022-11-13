#global testerror

//#[Swag.PrintBc]
#test
{
    a := 2'f32
    var b: const &s32 = &a
    @assert(b == 2)
}
