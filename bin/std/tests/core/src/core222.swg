using Core

#test
{
    var v: Math.NumericArray'(f32, 4)
    v = 1
    v += 10
    @assert(v[0] == 11)
    @assert(v[1] == 11)
    @assert(v[2] == 11)
    @assert(v[3] == 11)
}
