#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    var x: Array'String
    var y: String = "toto"

    //console.write("before: %\n", y)
    x.emplaceBack(&y)
    //console.write("after: %\n", y)
}