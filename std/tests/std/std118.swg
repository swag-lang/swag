#import "std"
using std.collections

#test
{
    var arr: HashTable'(String, s64)
    arr.clear()
    //arr.add(String.from("val0"), 10)
    //arr.add("val1", 11)

    x := String.from("toto")
    y := String.from("toto")
    @assert(x == y)
    y = "titi"
    @assert(x != y)
}