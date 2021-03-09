
using core

//#[swag.printbc]
#test
{
    alias arr4 = math.NumericArray'(f32, 4)
    var v1: arr4 = @[1, 2, 3, 4]
    var v2: arr4 = @[1, 2, 3, 4]
    v1.add(v2)
    @assert(v1 == arr4.from(2, 4, 6, 8))
}