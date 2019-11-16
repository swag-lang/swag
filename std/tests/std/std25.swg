#import "std"
using std

#test
{
    var x: String = "toto"
    x += "_titiあ"
    @assert(x == "toto_titiあ")
    console.silent(true)
    console.write(x)
    console.silent(false)
}