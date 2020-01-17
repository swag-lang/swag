#import "std"
using std

#test
{
    var x: String = "coucou\n"
    console.silent(true)
    console.write(cast(string) x)
    console.silent(false)
}