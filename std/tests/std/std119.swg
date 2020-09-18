#import "std"
using std.collections, std.text

func toto(v: String)
{
    res := format.toString("std119: %", v)
    @assert(res == "std119: yes this is a string")
    //console.write("%", res)
}

#test
{
    var x: String = "yes this is a string"
    toto(x)
}