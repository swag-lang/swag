//#[Swag.PrintBc]
#test
{
    var a  = 2
    var b: &s32 = &a
    b = 3
    @assert(a == 3)
    @assert(b == 3)
}
