#global testerror

//#[Swag.PrintBc]
#test
{
    a := 2'f32
    var b: const ref s32 = &a
    @assert(b == 2)
}
