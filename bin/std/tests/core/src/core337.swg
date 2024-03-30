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
    @assert(arr[0] == "0")
    @assert(arr[1] == "3")
    @assert(arr[2] == "4")
    @assert(arr[3] == "5")
    @assert(arr[4] == "1")
    @assert(arr[5] == "2")

    @assert(arr1[0].isNull())
    @assert(arr1[1].isNull())
    @assert(arr1[2].isNull())
}
