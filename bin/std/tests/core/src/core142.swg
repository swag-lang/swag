
using Core

#test
{
    var arr: Array's32 = @[1,2,3,4,5]
    @assert(arr.count == 5)
    @assert(arr[0] == 1)
    @assert(arr[4] == 5)
    arr.clear()
    @assert(arr.count == 0)
}
