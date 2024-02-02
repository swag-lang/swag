#global skip
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

#[Swag.Optim("", false)]
#[Swag.PrintBc]
#test
{
    var b: B
    var a: A
    a.ptr[0] = &b

    a[0].x = 4
    @print(b.x)
}