
struct(T) Array
{
    buffer:     ^T
    count:      u32
    size:       u32
}

impl Array
{
    func add(using self)
    {
        buffer[0] = 666
    }
}

//#[Swag.PrintBc]
#test
{
    var x    = 0
    var arr: Array's32
    arr.buffer = cast(^s32) &x
    arr.add()
    @assert(x == 666)
}
