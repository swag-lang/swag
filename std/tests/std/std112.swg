#import "std"
using std.crypto

#test
{
    @assert(hash32(50'u8) == 50)
    @assert(hash32(-1's8) == 0xFFFFFFFF)
    @assert(hash32(1.0'f32) == 0x3F800000)
    @assert(hash32(1.0'f64) == 0x3F800000)
    #assert(hash32(50'u8) == 50)
    #assert(hash32(-1's8) == 0xFFFFFFFF)
    #assert(hash32(1.0'f32) == 0x3F800000)
    #assert(hash32(1.0'f64) == 0x3F800000)

    v1 := hash32("this is a string")
    @assert(v1 != 0)

    let x : std.text.String = "toto"
    v2 := hash32(x)

    /*@print(cast(s64) v1)
    @print(" ")
    @print(cast(s64) v2)*/
}