using Core

#test
{
    var arr: Array'String
    arr.add(String.from("0"))
    arr.add(String.from("1"))
    arr.add(String.from("2"))

    arr.insertAt(1, String.from("3"))
    @assert(dref arr[0] == "0")
    @assert(dref arr[1] == "3")
    @assert(dref arr[2] == "1")
    @assert(dref arr[3] == "2")
}