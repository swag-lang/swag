#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    var arr: HashTable'(string, u32)
    arr.add("0", 0)
    arr.add("1", 1)
    @assert(arr.contains("0"))
    @assert(arr.contains("1"))
    arr.remove("0")
    @assert(arr.removed == 1)
    arr.add("0", 0)
    @assert(arr.removed == 0)
}