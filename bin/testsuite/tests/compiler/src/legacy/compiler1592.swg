struct A
{
    front: s32
}

struct B
{
    buffer: [2] A
}

impl B
{
    func opIndex(me, index: u64)->*A
    {
        return &me.buffer[index]
    }
}

#test
{
    var arr: B
    @assert(arr[0].front == 0)
}
