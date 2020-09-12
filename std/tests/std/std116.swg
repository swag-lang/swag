#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    var arr: HashTable'(string, u32)
    arr.add("0", 0)
    arr.add("1", 1)
}