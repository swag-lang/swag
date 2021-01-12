#import "core"
using core

#test
{
    var v: math.NumericArray'(f32, 4)
    v.setAll(1)
    v.add(10)
    @assert(v[0] == 11)
    @assert(v[1] == 11)
    @assert(v[2] == 11)
    @assert(v[3] == 11)
}