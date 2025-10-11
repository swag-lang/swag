struct A
{
    front: s32 = 666
}

struct B
{
    buffer: [2] A
}

impl B
{
    func opIndex(me, index: u64)->A
    {
        return me.buffer[index]
    }
}

//#[Swag.PrintBc]
#test
{
    var arr: B
    @assert(arr[0].front == 666)
    @assert(arr[1].front == 666)
    @assert(arr.buffer[0].front == 666)
    @assert(arr.buffer[1].front == 666)
}
