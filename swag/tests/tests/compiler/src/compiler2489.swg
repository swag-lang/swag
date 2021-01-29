struct AA
{
    v = 666's64
}

struct BB
{
    a, b, c: f32
    using base: *AA
}

#test
{
    var a: AA
    var x: BB
    x.base = &a

    ptr1 := cast(*AA) &x
    @assert(ptr1.v == 666)
}