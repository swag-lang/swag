using Core

//#[Swag.PrintBc]
#test
{
    alias arr4 = Math.NumericArray'(f32, 4)
    var v1: arr4 = [1, 2, 3, 4]
    let v2: arr4 = [1, 2, 3, 4]
    v1 += v2
    @assert(v1 == arr4.from(2, 4, 6, 8))
}
