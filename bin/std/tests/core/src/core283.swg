using Core, Core.Math

//#[Swag.PrintBc]
#test
{
    var v: Variant'([s32, bool])

    v = 666's32
    @assert(v == 666's32)
    @assert(v != 667's32)
    @assert(v <= 668's32)
    @assert(v >= 665's32)

    v = true
    @assert(v == true)
}
