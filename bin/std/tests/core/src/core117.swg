using Core

#test
{
    var arr: HashTable'(string, s64)
    arr.add("val0", 10)
    arr.add("val1", 11)

    var total = 0's64
    foreach k, v in arr:
        total += v

    @assert(total == 21)
}
