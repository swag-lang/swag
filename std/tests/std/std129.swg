#import "std"
using std.text, std.diagnostics

#test
{
    x := 0
    str := format("%", &x)
    console.write("string is %\n", str)
}