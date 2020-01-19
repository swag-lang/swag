#import "std"
using std.text

#test
{
    var x: String = "coucou\n"
    console.silent(true)
    console.write(cast(string) x)
    console.silent(false)
}