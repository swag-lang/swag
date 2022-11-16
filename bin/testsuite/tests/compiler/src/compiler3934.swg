//#[Swag.PrintBc]
#test
{
    a := 2
    var b: const ref s32 = &a
    @assert(b == 2)
}

#test
{
    a := 2
    var b: const ref s32 = &a
    a += 1
    @assert(b == 3)
}