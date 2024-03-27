using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'String
    arr.add(String.from("a"))
    arr.add(String.from("b"))
    arr.add(String.from("c"))
    arr.add(String.from("d"))
    arr.remove(1, 2)
    @assert(arr.count == 2)
    @assert(arr[1] == "d")
}