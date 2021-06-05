//#[Swag.printbc]
#test
{
    var x: [..] s32
    @assert(x == null)
    x = @[1, 2]
    @assert(x != null)
}