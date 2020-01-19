#import "std"
using std.text

#test
{
    var x: String = "toto"
    x += "_titiあ"
    @assert(x == "toto_titiあ")
    console.silent(true)
    console.write(cast(string) x)
    console.silent(false)
}