//#global skip
struct B
{
    x:   s32 = 6
}

struct A
{
    ptr: [1] *B
}

impl A
{
    mtd toto()->&*B
    {
        return &me.ptr[0]
    }
}

#[Swag.Optimize(.All, false)]
#test
{
    var b: B
    var a: A
    a.ptr[0] = &b

    let c = a.toto().x //= 4
    @assert(c == 6)
}
