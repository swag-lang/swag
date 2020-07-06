#import "std"
using std.diagnostics

#test
{
    console.silent(true)
    console.write("toto %\n", 666)
    console.silent(false)
}
