
using Core

#test
{
    var arr: HashTable'(string, s64)
    arr.add("val0", 10)
    arr.add("val1", 11)

    var total = 0's64
    visit k, v: arr do
        total += v

    @assert(total == 21)
}