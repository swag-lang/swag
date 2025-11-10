struct(T) B {}

impl B
{
    func opIndex(me, index: u64)->T
    {
        let x: T = 666 + cast(s32) index
        return x
    }
}

#test
{
    var arr: B's32

    let val0 = arr.opIndex(0)
    @assert(val0 == 666)
    let val1 = arr[0]
    @assert(val1 == 666)

    let val00 = arr.opIndex(10)
    @assert(val00 == 676)
    let val10 = arr[10]
    @assert(val10 == 676)
}
