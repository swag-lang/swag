struct(T) B
{
}

impl B
{
    func opIndex(using self, index: u64) -> T
    {
        var x: T = 666 + cast(s32) index
        return x
    }
}

#test
{
    var arr: B's32

    var val0 = arr.opIndex(0)
    @assert(val0 == 666)
    var val1 = arr[0]
    @assert(val1 == 666)

    var val00 = arr.opIndex(10)
    @assert(val00 == 676)
    var val10 = arr[10]
    @assert(val10 == 676)
}