//#global skip
struct B
{
    x: s32 = 6
}

struct A
{
    ptr: [1] *B
}

impl A
{
    mtd opIndex(i: u64)->&*B
    {
        return &ptr[0]
    }
}

#[Swag.Optimize("", false)]
#[Swag.Safety("", false)]
#test
{
    var b: B
    var a: A
    a.ptr[0] = &b

    @assert(b.x == 6)
    var idx = -1
    idx += 1
    a[idx].x = 4
    @assert(b.x == 4)
}