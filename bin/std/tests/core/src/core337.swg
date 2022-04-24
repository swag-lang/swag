using Core

#test
{
    var arr: Array'String
    arr.add(String.from("0"))
    arr.add(String.from("1"))
    arr.add(String.from("2"))

    var arr1: Array'String
    arr1.add(String.from("3"))
    arr1.add(String.from("4"))
    arr1.add(String.from("5"))

    arr.emplaceAt(1, arr1.toSlice())
    @assert(dref arr[0] == "0")
    @assert(dref arr[1] == "3")
    @assert(dref arr[2] == "4")
    @assert(dref arr[3] == "5")
    @assert(dref arr[4] == "1")
    @assert(dref arr[5] == "2")

    @assert(arr1[0].isNull())
    @assert(arr1[1].isNull())
    @assert(arr1[2].isNull())
}