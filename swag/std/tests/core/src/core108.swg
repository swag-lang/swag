#import "core"
using core

#test
{
    var x: Array'String
    x.addAny("toto")

    var y: Array'u32
    y.addAny(10'u32)
}