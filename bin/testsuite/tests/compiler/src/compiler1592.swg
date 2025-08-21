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
    func opIndex(using me, index: u64)->*A
    {
        return &buffer[index]
    }
}

#test
{
    var arr: B
    @assert(arr[0].front == 0)
}