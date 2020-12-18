#import "core"
using core

#test
{
    var x: String = "coucou\n"
    console.silent(true)
    console.print(cast(string) x)
    console.silent(false)
}