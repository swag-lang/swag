using Core.Hash

#test
{
    /*@assert(hash32(50'u8) == 50)
    @assert(hash32(-1's8) == 0xFFFFFFFF)
    @assert(hash32(1.0'f32) == 0x3F800000)
    @assert(hash32(1.0'f64) == 0x3F800000)
    #assert(hash32(50'u8) == 50)
    #assert(hash32(-1's8) == 0xFFFFFFFF)
    #assert(hash32(1.0'f32) == 0x3F800000)
    #assert(hash32(1.0'f64) == 0x3F800000)*/

    let v1 = hash32("this is a string")
    @assert(v1 != 0)

    var x: String = "this is a string"
    let v2 = hash32(x)

    @assert(v1 == v2)

    var y: String
    let v3 = hash32(y)
    @assert(v3 == 0)

    /*struct G
    {

    }

    var xx: G
    var v4 = hash32(xx)*/
}
