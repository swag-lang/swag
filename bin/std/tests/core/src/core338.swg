using Core

#test
{
    var arr: Array's32
    arr.add(0)
    arr.add(1)
    arr.add(2)
    arr.add(3)
    arr.removeAtOrdered(1, 2)
    @assert(arr.count == 2)
    @assert(arr[0] == 0)
    @assert(arr[1] == 3)
}