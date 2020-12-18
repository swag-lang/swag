#import "core"
using core

#test
{
    var x: String = "toto"
    x += "_titiあ"
    @assert(x == "toto_titiあ")
    console.silent(true)
    console.print(cast(string) x)
    console.silent(false)
}