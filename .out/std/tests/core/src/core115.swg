
using core

#test
{
    var arr1: Array'String
    var arr2: Array'String
    arr1.addAny("0")
    arr1.addAny("1")
    arr2.addAny("2")
    arr2.addAny("3")
    arr1.add(arr2.toSlice())
    @assert(arr1.count == 4)
    @assert(:arr1[2] == "2")
    @assert(:arr1[3] == "3")

    var arr3: Array'u8
    arr3.add(cast(const [..] u8) "tous")
    @assert(arr3.count == 4)
    @assert(arr3[0] == "t"'u8)
    @assert(arr3[1] == "o"'u8)
    @assert(arr3[2] == "u"'u8)
    @assert(arr3[3] == "s"'u8)
}