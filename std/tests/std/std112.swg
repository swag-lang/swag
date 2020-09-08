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
    @assert(hash32("this is a string") != 0)

    //let x : std.text.String = "toto"
    //hash32(x)
}