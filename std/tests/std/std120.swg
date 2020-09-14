#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    var arr: HashTable'(String, s64)
    arr.add(String.from("val0"), 10)
    @assert(arr.contains(String.from("val0")))
    arr.clear()
}