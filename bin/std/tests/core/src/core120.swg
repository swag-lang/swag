
using Core

#test
{
    var arr: HashTable'(String, s64)
    arr.add(String.from("val0"), 10)
    @assert(arr.contains(String.from("val0")))
    arr.clear()
}