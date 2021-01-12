#import "core"
using core

#test
{
    var v: math.NumericArray'(f32, 4)
    v.set(1)
    @assert(v[0] == 1)
    @assert(v[1] == 1)
    @assert(v[2] == 1)
    @assert(v[3] == 1)
}