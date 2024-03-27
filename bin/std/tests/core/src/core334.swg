using Core

#test
{
    var arr: Array's32
    arr.add(0's32)
    arr.add(1's32)
    arr.add(2's32)

    arr.insertAt(1, cast(const [..] s32) [3, 4, 5])
    @assert(arr[0] == 0)
    @assert(arr[1] == 3)
    @assert(arr[2] == 4)
    @assert(arr[3] == 5)
    @assert(arr[4] == 1)
    @assert(arr[5] == 2)
}