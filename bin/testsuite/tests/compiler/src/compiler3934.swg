//#[Swag.PrintBc]
#test
{
    var a  = 2
    let b: const &s32 = &a
    @assert(b == 2)
}

#test
{
    var a  = 2
    let b: const &s32 = &a
    a += 1
    @assert(b == 3)
}
