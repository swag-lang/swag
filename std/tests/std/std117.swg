#import "std"
using std.collections, std.text

#test
{
    var arr: HashTable'(string, s64)
    arr.add("val0", 10)
    arr.add("val1", 11)

    total := 0's64
    visit k, v: arr
        total += v

    @assert(total == 21)
}