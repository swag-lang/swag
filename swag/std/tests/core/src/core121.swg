#import "core"
using core

#test
{
    var arr: HashTable'(string, s64)
    arr.add("val0", 10)
    arr.add("val1", 50)
    it := arr.find("val0")
    @assert(it != null && it.value == 10)
    it = arr.find("val1")
    @assert(it != null && it.value == 50)
}