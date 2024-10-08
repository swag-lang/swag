using Core

#test
{
    var arr: HashTable'(string, s64)
    arr.add("val0", 10)
    arr.add("val1", 50)
    var it = arr.tryFind("val0")
    @assert(it != null and it.value == 10)
    it = arr.tryFind("val1")
    @assert(it != null and it.value == 50)
}
