using Core

#test
{
    var arr: Array'u32
    for i in 10'u32:
        arr.add(i)

    let result = Slice.findLinear(arr.toSlice(), 5'u32)
    @assert(result == 5)

    let result1 = Slice.findLinear(arr.toSlice(), 15'u32)
    @assert(result1 == Swag.U64.Max)

    //Console.write("result is %\n", res)
}
