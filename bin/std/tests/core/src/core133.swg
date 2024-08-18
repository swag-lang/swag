using Core

#test
{
    var arr: Array'u32
    loop 10:
        arr.add(cast(u32) #index)
    arr.sort()

    loop 10
    {
        let result = Slice.findSorted(@mkslice(arr.buffer, arr.count), cast(u32) #index)
        @assert(result == #index)
    }

    //Console.write("result is %\n", res)
}
