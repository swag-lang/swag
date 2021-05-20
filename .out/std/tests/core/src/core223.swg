
using Core

//#[Swag.printbc]
#test
{
    alias arr4 = Math.NumericArray'(f32, 4)
    var v: arr4 = @[1, 2, 3, 4]
    v.add(10)
    @assert(v[0] == 11)
    @assert(v[1] == 12)
    @assert(v[2] == 13)
    @assert(v[3] == 14)
}