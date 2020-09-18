#import "std"
using std.collections

#test
{
    var arr: HashTable'(string, u32)
    arr.add("0", 0)
    arr.add("1", 1)
    @assert(arr.contains("0"))
    @assert(arr.contains("1"))
    @assert(!arr.contains("2"))
    arr.remove("0")
    @assert(arr.deleted == 1)
    arr.add("0", 0)
    @assert(arr.deleted == 0)
}