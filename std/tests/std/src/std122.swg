#import "std"
using std

#test
{
    {
        var x: String = "toto"
        var y: String = "titi"
        swap(&x, &y)
        @assert(x == "titi")
        @assert(y == "toto")
    }

    {
        var x: s32 = 1
        var y: s32 = 2
        swap(&x, &y)
        @assert(x == 2)
        @assert(y == 1)
    }
}